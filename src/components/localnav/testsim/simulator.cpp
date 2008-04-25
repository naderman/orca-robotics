#include "simulator.h"
#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h>
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

Simulator::Simulator( const orcaice::Context &context,
                      const orca::PathFollower2dData &testPath )
    : testPath_(testPath),
      iterationNum_(0),
      context_(context)
{
    Ice::PropertiesPtr prop = context.properties();
    std::string prefix = context.tag();
    prefix += ".Config.Test.";

    maxLateralAcceleration_ = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxLateralAcceleraton", 1.0 );
    bool checkLateralAcceleration = orcaice::getPropertyAsIntWithDefault( prop, prefix+"CheckLateralAcceleration", 0 );
    bool checkDifferentialConstraints = orcaice::getPropertyAsIntWithDefault( prop, prefix+"CheckDifferentialConstraints", 0 );
    batchMode_ = orcaice::getPropertyAsIntWithDefault( prop, prefix+"BatchMode", 0 );

    cout<<"TRACE(simulator.cpp): batchMode_: " << batchMode_ << endl;

    ogMap_.offset().p.x = -WORLD_SIZE/2.0;
    ogMap_.offset().p.y = -WORLD_SIZE/2.0;
    ogMap_.offset().o   = 0.0;
    
    const double CELL_SIZE = 0.1;
    ogMap_.setMetresPerCellX( CELL_SIZE );
    ogMap_.setMetresPerCellY( CELL_SIZE );
    ogMap_.reallocate( (int)(WORLD_SIZE/CELL_SIZE), (int)(WORLD_SIZE/CELL_SIZE) );

    setupMap();

    // set up simulation parameters
    hydrosim2d::VehicleSimulator::Config vehicleSimConfig;
    vehicleSimConfig.robotRadius                  = ROBOT_RADIUS;
    vehicleSimConfig.timeStep                     = DELTA_T;
    vehicleSimConfig.checkDifferentialConstraints = checkDifferentialConstraints;
    vehicleSimConfig.checkLateralAcceleration     = checkLateralAcceleration;
    vehicleSimConfig.maxLinearAcceleration        = 1.0;
    vehicleSimConfig.maxRotationalAcceleration    = DEG2RAD(90.0);
    vehicleSimConfig.maxLateralAcceleration       = maxLateralAcceleration_;
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
    c->maxLateralAcceleration    = maxLateralAcceleration_;
    c->maxForwardAcceleration    = vehicleSimConfig.maxLinearAcceleration;
    c->maxReverseAcceleration    = vehicleSimConfig.maxLinearAcceleration;
    c->maxRotationalAcceleration = vehicleSimConfig.maxRotationalAcceleration;
    orcaobjutil::checkVehicleControlVelocityDifferentialDescription( *c );
    vehicleDescr_.control        = c;

    vehicleDescr_.platformToVehicleTransform = orcaobj::zeroFrame3d();
    orca::VehicleGeometryCylindricalDescription *g
        = new orca::VehicleGeometryCylindricalDescription;
    g->type = orca::VehicleGeometryCylindrical;
    g->radius = ROBOT_RADIUS;
    g->height = 2.0;

    g->vehicleToGeometryTransform = orcaobj::zeroFrame3d();
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
placeObstacle( hydroogmap::OgMap &ogMap,
               double x,
               double y,
               double radius )
{
    int gridX, gridY;
    ogMap.getCellIndices( x, y, gridX, gridY );

    int radiusInCellsX = (int)(radius / ogMap.metresPerCellX());
    int radiusInCellsY = (int)(radius / ogMap.metresPerCellY());
    for ( int i=-radiusInCellsX; i < radiusInCellsX; i++ )
    {
        for ( int j=-radiusInCellsY; j < radiusInCellsY; j++ )
        {
            ogMap.gridCell(gridX+i,gridY+j) = hydroogmap::CELL_OCCUPIED;
        }
    }
}

void
placeRoom( hydroogmap::OgMap &ogMap )
{
    double centreX = -14, centreY = 14;
    double widthX = 4, widthY = 4;
    double doorWidth = 1.0;

    // the corners in grid-coords
    int blX, blY;
    int tlX, tlY;
    int trX, trY;
    int brX, brY;
    ogMap.getCellIndices( centreX-widthX/2, centreY-widthY/2, blX, blY );
    ogMap.getCellIndices( centreX-widthX/2, centreY+widthY/2, tlX, tlY );
    ogMap.getCellIndices( centreX+widthX/2, centreY-widthY/2, brX, brY );
    ogMap.getCellIndices( centreX+widthX/2, centreY+widthY/2, trX, trY );

    // side walls
    for ( int yi=blY; yi <= tlY; yi++ )
    {
        ogMap.gridCell(blX,yi) = hydroogmap::CELL_OCCUPIED;
        ogMap.gridCell(brX,yi) = hydroogmap::CELL_OCCUPIED;
    }
    // top & bottom walls
    for ( int xi=blX; xi <= brX; xi++ )
    {
        ogMap.gridCell(xi,tlY) = hydroogmap::CELL_OCCUPIED;
        ogMap.gridCell(xi,brY) = hydroogmap::CELL_OCCUPIED;
    }

    // clear a doorway
    int doorLeftX, doorRightX, doorY;
    ogMap.getCellIndices( centreX-doorWidth/2, centreY-widthY/2, doorLeftX, doorY );
    ogMap.getCellIndices( centreX+doorWidth/2, centreY-widthY/2, doorRightX, doorY );

    for ( int xi=doorLeftX; xi <= doorRightX; xi++ )
        ogMap.gridCell(xi,doorY) = hydroogmap::CELL_EMPTY;
}

void
Simulator::setupMap()
{
    ogMap_.fill( 0 );

//#if 0
    placeObstacle( ogMap_, 7, 4, 0.5 );
    placeObstacle( ogMap_, -2, -4, 0.5 );
    placeObstacle( ogMap_, -7, -4, 0.5 );
    placeObstacle( ogMap_, -5, -9, 0.5 );
    placeObstacle( ogMap_, 0, -3, 0.5 );
    placeObstacle( ogMap_, -10, 0, 0.5 );

    placeObstacle( ogMap_, 12, -15, 0.5 );
    placeObstacle( ogMap_, 14, -18, 0.5 );
    placeObstacle( ogMap_, 16, -12, 0.5 );
    placeObstacle( ogMap_, 18, -16, 0.5 );
//#endif

    placeRoom( ogMap_ );
}

void 
Simulator::act( const hydronavutil::Velocity &cmd )
{
    if ( !batchMode_ )
    {
        cout<<"TRACE(simulator.cpp): received cmd: " << cmd << endl;
        cout<<"TRACE(simulator.cpp): pose: " << vehicleSimulator_->pose() << endl;
        cout<<"TRACE(simulator.cpp): ============= hit return to continue ============" << endl;
        getchar();
    }

    vehicleSimulator_->act( cmd );

    iterationNum_++;
    checkProgress( testPath_, *vehicleSimulator_, iterationNum_ );
}

orca::Time 
Simulator::time() const
{
    double sec = DELTA_T * iterationNum_;

    orca::Time t;
    t.seconds = (int)sec;
    t.useconds = (sec-(int)sec) * 1e6;

    return t;
}

}
