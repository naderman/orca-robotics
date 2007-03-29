#include "simulator.h"
#include <iostream>
#include <orcageom2d/geom2d.h>
#include <orcaice/orcaice.h>
#include <orcapathplan/pathplanutils.h>
#include <orcamisc/configutils.h>
#include <localnavutil/brosutil.h>

using namespace std;

namespace localnav {

namespace {
    const double WORLD_SIZE = 40.0;
    const double MAX_RANGE  = 80.0;
    const double DELTA_T    = 0.1;
    const double ROBOT_RADIUS = 0.375;
}

Simulator::Simulator( const orcaice::Context &context,
               const orca::PathFollower2dData &testPath )
    : rayTracer_(ogMap_),
      scan_(new orca::LaserScanner2dData),
      testPath_(testPath),
      iterationNum_(0),
      context_(context)
{
    // Set up pose
    pose_.x() = 0;
    pose_.y() = 0;
    pose_.theta() = 0;

    Ice::PropertiesPtr prop = context.properties();
    std::string prefix = context.tag();
    prefix += ".Config.Test.";

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
    grownOgMap_ = ogMap_;

    orcapathplan::growObstaclesOgMap( grownOgMap_,
                                      0.5,
                                      (int)(ROBOT_RADIUS/CELL_SIZE) );

    // setup scan
    scan_->timeStamp.seconds  = 0;
    scan_->timeStamp.useconds = 0;
    scan_->minRange = 0.0;
    scan_->maxRange = MAX_RANGE;
    scan_->fieldOfView = M_PI;
    scan_->startAngle  = -M_PI/2.0;
    scan_->ranges.resize(181);
    scan_->intensities.resize( scan_->ranges.size() );
    for ( uint i=0; i < scan_->ranges.size(); i++ )
        scan_->intensities[i] = 0;

    // setup velocity.
    // Start negative linear, to make sure the driver can hangle it.
    velLin_ = -1.0;
    velRot_ = 0.0;

    // setup interfaces
    setupInterfaces();

    // give an initial command
    orca::VelocityControl2dData cmd;
    cmd.motion.v.x = velLin_;
    cmd.motion.v.y = 0;
    cmd.motion.w   = velRot_;
    setCommand( cmd );
}

Simulator::~Simulator()
{
    delete laserInterface_;
    delete localiseInterface_;
    delete ogMapInterface_;
}

void
Simulator::setupInterfaces()
{
    scannerDescr_.minRange = 0.0;
    scannerDescr_.maxRange = MAX_RANGE;
    scannerDescr_.fieldOfView = M_PI;
    scannerDescr_.startAngle = -M_PI/2.0;
    scannerDescr_.numberOfSamples = 181;
    setToZero( scannerDescr_.offset );
    scannerDescr_.size.l = 0.1;
    scannerDescr_.size.w = 0.1;
    scannerDescr_.size.h = 0.1;
    scannerDescr_.offset.p.x = 0;
    scannerDescr_.offset.p.y = 0;
    scannerDescr_.offset.p.z = 0;
    scannerDescr_.offset.o.r = 0;
    scannerDescr_.offset.o.p = 0;
    scannerDescr_.offset.o.y = 0;
    scannerDescr_.timeStamp = orcaice::getNow();
    
    // record the description in the RangeScannerSensorDescription class
    rangeScannerSensorDescription_.setDescr( scannerDescr_ );
    
    laserInterface_    = new orcaifaceimpl::LaserScanner2dIface( scannerDescr_, "TestLaserScanner", context_ );
    orca::Localise2dDescription descr;
    descr.timeStamp = orcaice::getNow();
    descr.offset.p.x = 0;
    descr.offset.p.y = 0;
    descr.offset.o   = 0;
    localiseInterface_ = new orcaifaceimpl::Localise2dIface( "TestLocalise", descr, context_ );
    ogMapInterface_    = new orcaifaceimpl::OgMapIface( "TestOgMap", context_ );

    try {
        laserInterface_->initInterface();
    } catch (...) {}
    try {
        localiseInterface_->initInterface();
    } catch (...) {}
    try {
        cout<<"TRACE(simulator.cpp): Initialising og map" << endl;
        orca::OgMapData orcaOgMap;
        orcaogmap::convert( ogMap_, orcaOgMap );
        ogMapInterface_->initInterface();
        ogMapInterface_->localSetAndSend( orcaOgMap );
        cout<<"TRACE(simulator.cpp): done initialising og map." << endl;
        // cout<<"TRACE(simulator.cpp): map is: " << orcaice::toVerboseString(orcaOgMap) << endl;

    } catch (...) {}
}

ISensorDescription& 
Simulator::rangeScanner2dDescription()
{
    return rangeScannerSensorDescription_;
}

void
placeObstacle( orcaogmap::OgMap &ogMap,
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
            ogMap.gridCell(gridX+i,gridY+j) = orcaogmap::CELL_OCCUPIED;
        }
    }
}

void
placeRoom( orcaogmap::OgMap &ogMap )
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
        ogMap.gridCell(blX,yi) = orcaogmap::CELL_OCCUPIED;
        ogMap.gridCell(brX,yi) = orcaogmap::CELL_OCCUPIED;
    }
    // top & bottom walls
    for ( int xi=blX; xi <= brX; xi++ )
    {
        ogMap.gridCell(xi,tlY) = orcaogmap::CELL_OCCUPIED;
        ogMap.gridCell(xi,brY) = orcaogmap::CELL_OCCUPIED;
    }

    // clear a doorway
    int doorLeftX, doorRightX, doorY;
    ogMap.getCellIndices( centreX-doorWidth/2, centreY-widthY/2, doorLeftX, doorY );
    ogMap.getCellIndices( centreX+doorWidth/2, centreY-widthY/2, doorRightX, doorY );

    for ( int xi=doorLeftX; xi <= doorRightX; xi++ )
        ogMap.gridCell(xi,doorY) = orcaogmap::CELL_EMPTY;
}

void
Simulator::setupMap()
{
    ogMap_.fill( 0 );

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

    placeRoom( ogMap_ );
}

orca::Time
Simulator::now()
{
    double sec = 1+iterationNum_*DELTA_T;
    double useconds = fmod( sec, 1.0 ) * 1e6;

    orca::Time t;
    t.seconds  = (int)(sec);
    t.useconds = (int)(useconds);

    return t;
}

void 
Simulator::fillPipes()
{
    orca::Time ts = now();

    scan_->timeStamp = ts;
    obsProxy_.set(scan_);
    laserInterface_->localSetAndSend(scan_);

    orca::Localise2dData locData;
    locData.hypotheses.resize(1);
    locData.hypotheses[0].mean.p.x = pose_.x();
    locData.hypotheses[0].mean.p.y = pose_.y();
    locData.hypotheses[0].mean.o = pose_.theta();
    locData.hypotheses[0].cov.xx = 0.1;
    locData.hypotheses[0].cov.xy = 0.0;
    locData.hypotheses[0].cov.yy = 0.1;
    locData.hypotheses[0].cov.xt = 0.0;
    locData.hypotheses[0].cov.yt = 0.0;
    locData.hypotheses[0].cov.tt = 1.0*M_PI/180.0;
    locData.hypotheses[0].weight = 1.0;
    locData.timeStamp = ts;
    locProxy_.set(locData);
    localiseInterface_->localSetAndSend(locData);

    orca::Odometry2dData posData;
    posData.pose.p.x   = 0.0;
    posData.pose.p.y   = 0.0;
    posData.pose.o     = 0.0;
    posData.motion.v.x = velLin_;
    posData.motion.v.y = 0.0;
    posData.motion.w   = velRot_;
    posData.timeStamp  = ts;

    odomProxy_.set( posData );
}

void 
Simulator::setCommand( const orca::VelocityControl2dData &cmd )
{
//     cmd.timeStamp = scan_->timeStamp;
    
    if ( !batchMode_ )
    {
        cout<<"TRACE(simulator.cpp): iteration " << iterationNum_ << ": pose_: " << pose_ << endl;
        cout<<"TRACE(simulator.cpp): received cmd: " << orcaice::toString(cmd) << endl;
        cout<<"TRACE(simulator.cpp): pose_: " << pose_ << endl;
        cout<<"TRACE(simulator.cpp): ============= hit return to continue ============" << endl;
        getchar();
    }

    // First apply the current velocity
    applyCurrentVelocity();
    
    // Then set the new commanded velocity
    velLin_ = cmd.motion.v.x;
    velRot_ = cmd.motion.w;

    // Get a new observation
    getRanges();

    fillPipes();

    checkProgress();
}

bool
bordersFreeSpace( const orcaogmap::OgMap &ogMap, int x, int y )
{
    return ( ogMap.gridCell( x-1, y ) < orcaogmap::CELL_UNKNOWN ||
             ogMap.gridCell( x+1, y ) < orcaogmap::CELL_UNKNOWN ||
             ogMap.gridCell( x, y-1 ) < orcaogmap::CELL_UNKNOWN ||
             ogMap.gridCell( x, y+1 ) < orcaogmap::CELL_UNKNOWN );
}

void
Simulator::checkProgress()
{
    assert( testPath_.path.size() > 0 );

    iterationNum_++;

    const int MIN_NUM_ITERATIONS = 5;
    const int MAX_NUM_ITERATIONS = testPath_.path.size()*400;

    double distanceToGoal = hypotf( pose_.y()-testPath_.path.back().target.p.y,
                                    pose_.x()-testPath_.path.back().target.p.x );
    double angleToGoal = pose_.theta()-testPath_.path.back().target.o;
    NORMALISE_ANGLE( angleToGoal );
    angleToGoal = fabs( angleToGoal );
    bool lastGoalReached = ( distanceToGoal <= testPath_.path.back().distanceTolerance &&
                             angleToGoal <= testPath_.path.back().headingTolerance );

    if ( iterationNum_ < MIN_NUM_ITERATIONS && lastGoalReached )
    {
        cout << "ERROR(simulator.cpp): Huh? How did we reach the goal in only "<< iterationNum_ << " iterations??" << endl;
        exit(1);
    }
    if ( iterationNum_ >= MAX_NUM_ITERATIONS && !lastGoalReached )
    {
        cout << "ERROR(simulator.cpp): Failed: didn't reach the goal in " <<  iterationNum_ << " iterations." << endl;
        exit(1);        
    }
    if ( lastGoalReached )
    {
        cout<<"TRACE(simulator.cpp): Final Goal reached.  Test passed." << endl;
        exit(0);
    }

    // Check for collisions
    bool coordsWithinMap = grownOgMap_.coordsWithinMap( pose_.x(), pose_.y() );
    if ( !coordsWithinMap )
    {
        cout<<"TRACE(simulator.cpp): Vehicle left the map!" << endl;
        exit(1);
    }
    else if ( grownOgMap_.worldCell( pose_.x(), pose_.y() ) > orcaogmap::CELL_UNKNOWN )
    {
        // Cell is occupied.  To be really, sure, make sure we're _inside_ an obstacle
        // (discrectisation can screw with things, so give the controller the beenfit of the doubt).
        int cellX, cellY;
        grownOgMap_.getCellIndices( pose_.x(), pose_.y(), cellX, cellY );

        if ( !bordersFreeSpace( grownOgMap_, cellX, cellY ) )
        {
            cout << "ERROR(simulator.cpp): Collision!!" << endl;
            exit(1);
        }
    }
}

void
Simulator::applyCurrentVelocity()
{
    orcanavutil::addPoseOffset( pose_.x(),
                                pose_.y(),
                                pose_.theta(),
                                velLin_*DELTA_T,
                                0,
                                velRot_*DELTA_T,
                                true );
}

void
Simulator::getRanges()
{
    double maxRange = scan_->maxRange;
    double angleIncrement = orcaice::calcAngleIncrement( scan_->fieldOfView,
                                                         scan_->ranges.size() );
    for ( uint i=0; i < scan_->ranges.size(); i++ )
    {
        double angleRCS = scan_->startAngle + i*angleIncrement;
        double angleGCS = angleRCS + pose_.theta();

        geom2d::Point start( pose_.x(), pose_.y() );
        geom2d::Point maxEnd( start.x() + maxRange * cos(angleGCS),
                              start.y() + maxRange * sin(angleGCS) );
        

        // ray-trace
        geom2d::Point actualEnd;
        rayTracer_.isClearWorld( start.x(), start.y(),
                                 maxEnd.x(), maxEnd.y(),
                                 actualEnd.x(), actualEnd.y() );

        double actualRange = hypotf( actualEnd.y()-start.y(), actualEnd.x()-start.x() );
        scan_->ranges[i] = actualRange;
    }
}

orca::VehicleDescription 
Simulator::getVehicleDescription() const
{
    orca::VehicleDescription d;

    orca::VehicleControlVelocityDifferentialDescription *c 
        = new orca::VehicleControlVelocityDifferentialDescription;
    c->type    = orca::VehicleControlVelocityDifferential;
    c->maxForwardSpeed = 20.0;
    c->maxReverseSpeed = 20.0;
    c->maxTurnrate     = DEG2RAD(990.0);
    c->maxTurnrateAtMaxSpeed  = DEG2RAD(990.0);
    c->maxForwardAcceleration = 1.0;
    c->maxReverseAcceleration = 1.0;
    c->maxRotationalAcceleration = DEG2RAD(90.0);
    orcamisc::checkVehicleControlVelocityDifferentialDescription( *c );
    d.control = c;

    setToZero( d.platformToVehicleTransform );

    orca::VehicleGeometryCylindricalDescription *g
        = new orca::VehicleGeometryCylindricalDescription;
    g->type = orca::VehicleGeometryCylindrical;
    g->radius = ROBOT_RADIUS;
    g->height = 2.0;

    setToZero( g->vehicleToGeometryTransform );
    d.geometry = g;

    return d;
}

void
Simulator::printState()
{
    cout<<"TRACE(simulator.cpp): pose: " << pose_ << endl;

}

}
