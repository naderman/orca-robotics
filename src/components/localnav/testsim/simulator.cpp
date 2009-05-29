#include "simulator.h"
#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaobj/bros1.h>
#include <orcaobjutil/vehicleutil.h>
#include <hydropathplan/hydropathplan.h>
#include <hydrogeom2d/geom2d.h>
#include <orcaogmap/orcaogmap.h>
#include "testsimutil.h"

using namespace std;

namespace localnav {

namespace {
    const double WORLD_SIZE = 40.0;
    const double MAX_RANGE  = 80.0;
    const double DELTA_T    = 0.1; // sec
    const double ROBOT_RADIUS = 0.375;
}

Simulator::Simulator( const orcaice::Context         &context,
                      const hydroogmap::OgMap        &ogMap, 
                      const orca::PathFollower2dData &testPath,
                      const Config                   &config )
    : ogMap_(ogMap),
      testPath_(testPath),
      iterationNum_(0),
      wpI_(0),
      config_(config),
      context_(context)
{
    cout<<"TRACE(simulator.cpp): Config: " << toString(config) << endl;

    // set up simulation parameters
    hydrosim2d::VehicleSimulator::Config vehicleSimConfig;
    vehicleSimConfig.robotRadius                  = ROBOT_RADIUS;
    vehicleSimConfig.timeStep                     = DELTA_T;
    vehicleSimConfig.checkDifferentialConstraints = config_.checkDifferentialConstraints;
    vehicleSimConfig.checkLateralAcceleration     = config_.checkLateralAcceleration;
    vehicleSimConfig.maxLinearAcceleration        = 1.0;
    vehicleSimConfig.maxRotationalAcceleration    = DEG2RAD(90.0);
    vehicleSimConfig.maxLateralAcceleration       = config_.maxLateralAcceleration;
    vehicleSimConfig.checkVelocityConstraints     = config_.checkVelocityConstraints;
    vehicleSimConfig.minVelocity.lin()            = 0.0;
    vehicleSimConfig.maxVelocity.lin()            = 20.0;
    vehicleSimConfig.minVelocity.rot()            = DEG2RAD(-1000.0);
    vehicleSimConfig.maxVelocity.rot()            = DEG2RAD( 1000.0);

    hydrosim2d::RangeScannerSimulator::Config rangeScanSimConfig;
    rangeScanSimConfig.maxRange       = 80.0;
    rangeScanSimConfig.startAngle     = DEG2RAD(-90.0);
    rangeScanSimConfig.angleIncrement = DEG2RAD(1.0);
    rangeScanSimConfig.numReturns     = 181;

    // set up interfaces
    setupInterfaces( vehicleSimConfig, rangeScanSimConfig );

    // instantiate the simulation
    vehicleSimulator_.reset( new hydrosim2d::VehicleSimulator( vehicleSimConfig,
                                                               ogMap_,
                                                               *posePublisher_ ) );
    rangeScannerSimulator_.reset( new hydrosim2d::RangeScannerSimulator( rangeScanSimConfig,
                                                                         ogMap_,
                                                                         *rangeScanPublisher_ ) );
}

void
Simulator::setupInterfaces( const hydrosim2d::VehicleSimulator::Config &vehicleSimConfig,
                            const hydrosim2d::RangeScannerSimulator::Config rangeScanSimConfig )
{
    scannerDescr_.minRange        = 0.0;
    scannerDescr_.maxRange        = rangeScanSimConfig.maxRange;
    scannerDescr_.fieldOfView     = 
        (rangeScanSimConfig.numReturns-1)*rangeScanSimConfig.angleIncrement;
    scannerDescr_.startAngle      = rangeScanSimConfig.startAngle;
    scannerDescr_.numberOfSamples = rangeScanSimConfig.numReturns;
    scannerDescr_.offset          = orcaobj::zeroFrame3d();
    scannerDescr_.size.l          = 0.1;
    scannerDescr_.size.w          = 0.1;
    scannerDescr_.size.h          = 0.1;
    scannerDescr_.offset.p.x      = 0;
    scannerDescr_.offset.p.y      = 0;
    scannerDescr_.offset.p.z      = 0;
    scannerDescr_.offset.o.r      = 0;
    scannerDescr_.offset.o.p      = 0;
    scannerDescr_.offset.o.y      = 0;
    scannerDescr_.timeStamp       = orcaice::getNow();

    rangeScanPublisher_.reset( new orcasim2d::RangeScanPublisher( scannerDescr_,
                                                                  "TestLaserScanner",
                                                                  context_ ) );

    orca::VehicleControlVelocityDifferentialDescription *c 
        = new orca::VehicleControlVelocityDifferentialDescription;
    c->type                      = orca::VehicleControlVelocityDifferential;
    c->maxForwardSpeed           = vehicleSimConfig.maxVelocity.lin();
    c->maxReverseSpeed           = vehicleSimConfig.minVelocity.lin();
    c->maxTurnrate               = vehicleSimConfig.maxVelocity.rot();
    c->maxLateralAcceleration    = config_.maxLateralAcceleration;
    c->maxForwardAcceleration    = vehicleSimConfig.maxLinearAcceleration;
    c->maxReverseAcceleration    = vehicleSimConfig.maxLinearAcceleration;
    c->maxRotationalAcceleration = vehicleSimConfig.maxRotationalAcceleration;
    orcaobjutil::checkVehicleControlVelocityDifferentialDescription( *c );
    vehicleDescr_.control        = c;

    orca::VehicleGeometryCylindricalDescription *g
        = new orca::VehicleGeometryCylindricalDescription;
    g->type = orca::VehicleGeometryCylindrical;
    g->radius = ROBOT_RADIUS;
    g->height = 2.0;

    g->platformToGeometryTransform = orcaobj::zeroFrame3d();
    vehicleDescr_.geometry = g;

    posePublisher_.reset( new orcasim2d::PosePublisher( vehicleDescr_.geometry,
                                                        "TestLocalise",
                                                        context_ ) );

    ogMapInterface_    = new orcaifaceimpl::OgMapImpl( "TestOgMap",
                                                        context_ );
    try {
        rangeScanPublisher_->initInterface();
    } 
    catch ( std::exception &e ) {
        cout << "Ignoring problem initialising interface: " << e.what();
    }

    try {
        posePublisher_->initInterface();
    }
    catch ( std::exception &e ) {
        cout << "Ignoring problem initialising interface: " << e.what();
    }

    try {
        orca::OgMapData orcaOgMap;
        orcaogmap::convert( ogMap_, orcaOgMap );
        ogMapInterface_->initInterface();
        ogMapInterface_->localSetAndSend( orcaOgMap );
    }
    catch ( std::exception &e ) {
        cout << "Ignoring problem initialising interface: " << e.what();
    }
}

void 
Simulator::act( const hydronavutil::Velocity &cmd )
{
    if ( iterationNum_ % 50 == 0 )
    {
        cout<<"TRACE(simulator.cpp): iteration: " << iterationNum_ << endl;
    }
    if ( !config_.batchMode && iterationNum_ >= config_.numIterationsBatch )
    {
        cout<<"TRACE(simulator.cpp): received cmd: " << cmd << endl;
        cout<<"TRACE(simulator.cpp): pose: " << vehicleSimulator_->pose() << endl;
        cout<<"TRACE(simulator.cpp): ============= hit return for iteration " << iterationNum_ << " ============" << endl;
        getchar();
    }
    else
    {
//        cout<<"TRACE(simulator.cpp): PAUSING..." << endl;
//        usleep(50000);
//        usleep(500000);
    }

    try {
        vehicleSimulator_->act( cmd );
    }
    catch ( std::exception &e )
    {
        cout << __func__ << "(): " << e.what() << endl;
        cout<<"TRACE(simulator.cpp): test FAILED" << endl;
        exit(1);
    }

    iterationNum_++;
}

void
Simulator::checkProgress( bool &pathCompleted, bool &pathFailed )
{
    localnav::checkProgress( testPath_,
                             *vehicleSimulator_,
                             iterationNum_,
                             config_.numIterationsLimit,
                             wpI_,
                             pathCompleted,
                             pathFailed );
}

orca::Time 
Simulator::time() const
{
    double sec = DELTA_T * iterationNum_;

    orca::Time t;
    t.seconds = (int)sec;
    t.useconds = (int)((sec-(int)sec) * 1e6);

    return t;
}

std::string toString( const Simulator::Config &c )
{
    stringstream ss;
    ss << endl;
    ss << "maxLateralAcceleration       : " << c.maxLateralAcceleration << endl;
    ss << "checkLateralAcceleration     : " << c.checkLateralAcceleration << endl;
    ss << "checkDifferentialConstraints : " << c.checkDifferentialConstraints << endl;
    ss << "useRoom                      : " << c.useRoom << endl;
    ss << "batchMode                    : " << c.batchMode << endl;
    ss << "numIterationsBatch           : " << c.numIterationsBatch << endl;
    ss << "numIterationsLimit           : " << c.numIterationsLimit << endl;
    return ss.str();
}

}
