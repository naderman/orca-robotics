#include "simulator.h"
#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaobj/bros1.h>
#include <orcaobjutil/vehicleutil.h>
#include <hydrogeom2d/geom2d.h>
#include <orcaogmap/orcaogmap.h>
#include "testsimutil.h"

using namespace std;

namespace orcalocalnavtest {

namespace {
    const double WORLD_SIZE = 40.0;
    const double MAX_RANGE  = 80.0;
    const double DELTA_T    = 0.1; // sec
    const double ROBOT_RADIUS = 0.375;
}

Simulator::Simulator( hydropublish::RangeScanner2dPublisherFactory &rangeScanner2dPublisherFactory,
                      hydropublish::Localise2dPublisherFactory     &localise2dPublisherFactory,
                      hydropublish::Odometry2dPublisherFactory     &odometry2dPublisherFactory,
                      hydropublish::OgMapPublisherFactory          &ogMapPublisherFactory,
                      const hydroogmap::OgMap                      &ogMap, 
                      const orca::PathFollower2dData               &testPath,
                      const Config                                 &config )
    : ogMap_(ogMap),
      testPath_(testPath),
      iterationNum_(0),
      wpI_(0),
      config_(config)
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
    vehicleSimConfig.minVelocity.lin()            = -1.0;
    vehicleSimConfig.maxVelocity.lin()            = 20.0;
    vehicleSimConfig.minVelocity.rot()            = DEG2RAD(-1000.0);
    vehicleSimConfig.maxVelocity.rot()            = DEG2RAD( 1000.0);
    vehicleSimConfig.initialPose                  = config_.initialPose;
    vehicleSimConfig.applyNoises                  = config_.applyNoises;
    vehicleSimConfig.noises                       = config_.noises;

    hydrosim2d::RangeScannerSimulator::Config rangeScanSimConfig;
    rangeScanSimConfig.maxRange       = 80.0;
    rangeScanSimConfig.startAngle     = DEG2RAD(-90.0);
    rangeScanSimConfig.angleIncrement = DEG2RAD(1.0);
    rangeScanSimConfig.numReturns     = 181;

    // set up interfaces
    setupInterfaces( vehicleSimConfig,
                     rangeScanSimConfig,
                     rangeScanner2dPublisherFactory,
                     localise2dPublisherFactory,
                     odometry2dPublisherFactory,
                     ogMapPublisherFactory );

    // instantiate the simulation
    vehicleSimulator_.reset( new hydrosim2d::VehicleSimulator( vehicleSimConfig,
                                                               ogMap_,
                                                               posePublisher_.get(),
                                                               odomPublisher_.get() ) );
    rangeScannerSimulator_.reset( new hydrosim2d::RangeScannerSimulator( rangeScanSimConfig,
                                                                         ogMap_,
                                                                         *rangeScanPublisher_ ) );
}

void
Simulator::setupInterfaces( const hydrosim2d::VehicleSimulator::Config      &vehicleSimConfig,
                            const hydrosim2d::RangeScannerSimulator::Config &rangeScanSimConfig,
                            hydropublish::RangeScanner2dPublisherFactory    &rangeScanner2dPublisherFactory,
                            hydropublish::Localise2dPublisherFactory        &localise2dPublisherFactory,
                            hydropublish::Odometry2dPublisherFactory        &odometry2dPublisherFactory,
                            hydropublish::OgMapPublisherFactory             &ogMapPublisherFactory )
{
    //
    // Scanner
    //
    scannerConfig_ = hydroscanutil::ScannerConfig ( rangeScanSimConfig.maxRange,
                                                    rangeScanSimConfig.startAngle,
                                                    rangeScanSimConfig.angleIncrement,
                                                    rangeScanSimConfig.numReturns );
    rangeScanPublisher_ = rangeScanner2dPublisherFactory.createPublisher( scannerConfig_,
                                                                          config_.scannerOffset,
                                                                          scannerInterfaceName() );

    //
    // Vehicle
    //
    geomDescr_.height = 2.0;
    geomDescr_.radius = ROBOT_RADIUS;

    controlDescr_.maxForwardSpeed           = vehicleSimConfig.maxVelocity.lin();
    controlDescr_.maxReverseSpeed           = -vehicleSimConfig.minVelocity.lin();
    controlDescr_.maxTurnrate               = vehicleSimConfig.maxVelocity.rot();
    controlDescr_.maxLateralAcceleration    = vehicleSimConfig.maxLateralAcceleration;
    controlDescr_.maxForwardAcceleration    = vehicleSimConfig.maxLinearAcceleration;
    controlDescr_.maxReverseAcceleration    = vehicleSimConfig.maxLinearAcceleration;
    controlDescr_.maxRotationalAcceleration = vehicleSimConfig.maxRotationalAcceleration;

    posePublisher_ = localise2dPublisherFactory.createPublisher( geomDescr_,
                                                                 localiseInterfaceName() );
    odomPublisher_ = odometry2dPublisherFactory.createPublisher( controlDescr_,
                                                                 geomDescr_,
                                                                 odomInterfaceName() );
    ogMapPublisher_ = ogMapPublisherFactory.createPublisher( "TestOgMap" );

    try {
        rangeScanPublisher_->init();
    }
    catch ( std::exception &e ) {
        cout << "Ignoring problem initialising rangeScanPublisher_: " << e.what();
    }

    try {
        posePublisher_->init();
    }
    catch ( std::exception &e ) {
        cout << "Ignoring problem initialising posePublisher_: " << e.what();
    }

    try {
        odomPublisher_->init();
    }
    catch ( std::exception &e ) {
        cout << "Ignoring problem initialising odomPublisher_: " << e.what();
    }

    try {
        ogMapPublisher_->init();
        ogMapPublisher_->localSetAndSend( ogMap_ );
    }
    catch ( std::exception &e ) {
        cout << "Ignoring problem initialising ogMapPublisher_: " << e.what();
    }
}

void 
Simulator::act( const hydronavutil::Velocity &cmd )
{
    iterationNum_++;

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
        vehicleSimulator_->act( cmd, time() );
    }
    catch ( std::exception &e )
    {
        cout << __func__ << "(): " << e.what() << endl;
        cout<<"TRACE(simulator.cpp): test FAILED" << endl;
        exit(1);
    }
}

void
Simulator::checkProgress( bool &pathCompleted, bool &pathFailed )
{
    orcalocalnavtest::checkProgress( testPath_,
                                     *vehicleSimulator_,
                                     iterationNum_,
                                     config_.numIterationsLimit,
                                     wpI_,
                                     pathCompleted,
                                     pathFailed );
}

hydrotime::Time 
Simulator::time() const
{
    double sec = DELTA_T * iterationNum_;

    return hydrotime::Time( (int)sec, (int)((sec-(int)sec) * 1e6) );
}

void
Simulator::getObsRanges( std::vector<float> &obsRanges )
{
    hydronavutil::Pose scannerPose;
    const bool normaliseHeading = true;
    hydronavutil::addPoseOffset( vehicleSimulator_->pose(),
                                 config_.scannerOffset,
                                 scannerPose,
                                 normaliseHeading );
    rangeScannerSimulator_->getRangesFromPose( scannerPose, obsRanges, time() );
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
    ss << "applyNoises                  : " << c.applyNoises << endl;
    if ( c.applyNoises )
        ss << "noises                       : " << c.noises << endl;
    return ss.str();
}

}
