/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include <orcaice/orcaice.h>
#include <orcaifaceutil/pathplanner2d.h>
#include <orcaobj/localise2d.h>
#include <orcaobj/pathfollower2d.h>
#include <orcaobj/pathplanner2d.h>
#include <orcaogmap/orcaogmap.h>
#include "pathfollower2dI.h"

#include "mainthread.h"


using namespace std;

namespace goalplanner {

namespace {

    // Exceptions thrown/caught internally.
    // If isTemporary we'll hopefully be able to recover soon.
    class GoalPlanException : public std::exception
    { 
    public:
        GoalPlanException(const char *message, bool isTemporary)
            : message_(message),
              isTemporary_(isTemporary)
            {}
        GoalPlanException(const std::string &message, bool isTemporary)
            : message_(message),
              isTemporary_(isTemporary)
            {}
        ~GoalPlanException()throw(){}
        virtual const char* what() const throw() { return message_.c_str(); }
        bool isTemporary() const throw() { return isTemporary_; }
    private:
        std::string  message_;
        bool isTemporary_;
    };

    ///////////////////////////////////////////////////

    // Computes waypoint to start from
    // This is for the pathplanner's purposes, not the pathfollower.
    // So we don't care too much about tolerances and speeds.
    orca::Waypoint2d computeFirstWaypointForPathPlanning( const hydronavutil::Pose &pose )
    {
        orca::Waypoint2d wp;

        wp.target.p.x = pose.x();
        wp.target.p.y = pose.y();
        wp.target.o   = pose.theta();

        // add bogus tolerances and speeds
        wp.distanceTolerance = (Ice::Float)0.1;
        wp.headingTolerance  = (Ice::Float)(M_PI/2.0);
        wp.timeTarget        = 0.0;
        wp.maxApproachSpeed    = 2000;
        wp.maxApproachTurnrate = (float)DEG2RAD(2000); 

        return wp;
    }

    // Adjust timing: work out how long it takes to the first waypoint based on straight-line distance 
    // and configured velocityToFirstWaypoint_. Take the max of first wp time and the computed time.
    // Add this time to all waypoints.
    void addTimeToReachFirstWp( const hydronavutil::Pose &pose, 
                                double velocityToFirstWaypoint,
                                orca::PathFollower2dData &incomingPath )
    {  
        // compute time to reach 1st wp based on straight-line distance: timeDist
        assert( incomingPath.path.size()>0 );
        orca::Waypoint2d &firstWp = incomingPath.path[0];
        double dX = pose.x()-firstWp.target.p.x;
        double dY = pose.y()-firstWp.target.p.y;
        double dist = sqrt( dX*dX + dY*dY );
        double timeDist = dist/velocityToFirstWaypoint;
        
        // time to reach 1st waypoint according to incoming path: timeIn
        double timeIn = firstWp.timeTarget;
        
        // compute difference between the two
        double timeOffset =  timeDist - timeIn;
        
        // nothing to change if timeIn is bigger than timeDist
        if (timeOffset<0.0) return; 
        
        // otherwise: add the offset to all waypoints in the path
        for (unsigned int i=0; i<incomingPath.path.size(); i++)
        {
            orca::Waypoint2d &wp = incomingPath.path[i];
            wp.timeTarget = wp.timeTarget + timeOffset;
        }
    }

    hydronavutil::Pose mlPose( const orca::Localise2dData &localiseData )
    {
        const orca::Pose2dHypothesis &mlHyp = orcaobj::mlHypothesis( localiseData );
        return hydronavutil::Pose( mlHyp.mean.p.x, mlHyp.mean.p.y, mlHyp.mean.o );
    }

    bool localisationIsUncertain( const orca::Localise2dData &localiseData )
    {
        if ( localiseData.hypotheses.size() > 1 )
            return true;
        if ( localiseData.hypotheses.size() == 0 )
        {
            stringstream ss;
            ss << "Badly formed localiseData: " << orcaobj::toString( localiseData );
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }
        
        const orca::Pose2dHypothesis &h = localiseData.hypotheses[0];

        const double LIN_THRESHOLD = 2.0;
        const double ROT_THRESHOLD = M_PI;
        
        return
            ( h.cov.xx > LIN_THRESHOLD ||
              h.cov.yy > LIN_THRESHOLD ||
              h.cov.tt > ROT_THRESHOLD );
    }

    double ageOf( const orca::Time &ts )
    {
        return orcaice::timeDiffAsDouble( orcaice::getNow(), ts );
    }

    // check whether the localise data is stale, if yes throws an exception
    void checkForStaleness( orca::Localise2dData &data )
    {
        const double AGE_FOR_STALE = 3; // seconds
        if ( ageOf( data.timeStamp ) > AGE_FOR_STALE )
        {
            stringstream ss;
            ss << "MainThread: LocaliseData is stale: age is " << ageOf( data.timeStamp ) << " sec";
            bool isTemporary = true;
            throw GoalPlanException( ss.str(), isTemporary );
        }
    }

    double distance( const hydronavutil::Pose &pose, const orca::Waypoint2d &wp )
    {
        return hypot( pose.y()-wp.target.p.y, pose.x()-wp.target.p.x );
    }
    double distance( const orca::Waypoint2d &wp1, const orca::Waypoint2d &wp2 )
    {
        return hypot( wp1.target.p.y-wp2.target.p.y, wp1.target.p.x-wp2.target.p.x );
    }

    orca::PathFollower2dData
    toPathFollowerData( const orca::PathPlanner2dData &pathPlan )
    {
        orca::PathFollower2dData outgoingPath;
        outgoingPath.timeStamp = pathPlan.timeStamp;
        outgoingPath.path = pathPlan.path;
        // get rid of first waypoint, it's the robot's location which is not needed
        vector<orca::Waypoint2d>::iterator it = outgoingPath.path.begin();
        outgoingPath.path.erase(it);

        return outgoingPath;
    }

    void
    modifyNextWaypointTolerance( orca::Waypoint2d         &wp,
                                 const hydronavutil::Pose &pose )
    {
        double distToWpCentre = hypot( wp.target.p.y - pose.y(),
                                       wp.target.p.x - pose.x() );
        if ( distToWpCentre < 2*wp.distanceTolerance )
        {
            const double MIN_DISTANCE_TOLERANCE = 0.3;
            wp.distanceTolerance = MAX( MIN_DISTANCE_TOLERANCE, distToWpCentre/2.0 );
        }
    }
}

///////////////////////////////////

MainThread::MainThread( const orcaice::Context & context )
    : orcaice::SubsystemThread( context.tracer(), context.status(), "MainThread" ),
      incomingPathI_(0),
      replanRequested_(false),
      context_(context)
{
}

void 
MainThread::initialise()
{
    setMaxHeartbeatInterval( 10.0 );

    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag()+".Config.";
    pathPlanTimeout_ = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"PathPlanTimeout", 10.0 );
    velocityToFirstWaypoint_ = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"VelocityToFirstWaypoint", 1.0 );
    checkForStaleLocaliseData_ = orcaice::getPropertyAsIntWithDefault( prop, prefix+"CheckForStaleLocaliseData", 1 );
    enableReplan_ = orcaice::getPropertyAsIntWithDefault( prop, prefix+"EnableReplan", 0 );
    requiredReplanRequestTime_ = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"RequiredReplanRequestTime", 2.0 );

    initNetwork();
}

void 
MainThread::work()
{
    const int timeoutMs = 1000;
    setMaxHeartbeatInterval( timeoutMs/1e3 );

    orca::PathFollower2dData incomingPath;
    bool requestIsOutstanding = false;

    // main loop
    while ( !isStopping() )
    {
        try
        {
            // If we have an outstanding request, service it now
            // (but check to see if we should over-write with a new one)
            // Otherwise, wait for a new request.
            if ( requestIsOutstanding )
            {
                if ( !incomingPathStore_.isEmpty() )
                {
                    // Overwrite the unserviced request with the new one.
                    incomingPathStore_.get( incomingPath );
                }
            }
            else
            {
                //
                // This will wait until the new path arrives
                //
                context_.tracer().info("Waiting for a goal path");
                bool gotNewPath = waitForNewPath( incomingPath );
                if ( gotNewPath )
                {
                    cout<<"TRACE(mainthread.cpp): Got a new path." << endl;
                    requestIsOutstanding = true;
                }
                else
                {
                    // waitForNewPath will only exit without a new path if we're stopping.
                    assert( isStopping() );
                    break;
                }
            }

            stringstream ssPath;
            ssPath << "MainThread: Received path request: " << endl << orcaobj::toVerboseString(incomingPath);
            context_.tracer().debug( ssPath.str() );

            string sketchReason;
            if ( orcaobj::isPathSketchy( incomingPath.path, sketchReason ) )
            {
                stringstream ss;
                ss << "Sketchy path: " << orcaobj::toVerboseString(incomingPath) << endl << "  " << sketchReason;
                health().warning( ss.str() );
            }

            // special case 'stop': we received an empty path
            if (incomingPath.path.size()==0) 
            {
                cout<<"TRACE(mainthread.cpp): Received an empty path; stopping robot." << endl;
                stopRobot();
                health().ok();
                requestIsOutstanding = false;
                continue;
            }

            // get robot pose
            bool isLocalisationUncertain;
            const hydronavutil::Pose pose = getPose(isLocalisationUncertain);
//             if ( isLocalisationUncertain )
//             {
//                 stringstream ss;
//                 ss << "MainThread: Localisation is too uncertain.";
//                 const bool isTemporary = true;
//                 throw GoalPlanException( ss.str(), isTemporary );
//             }
            
            // Adjust timing: work out how long it takes to the first waypoint based on straight-line distance 
            // and configured velocityToFirstWaypoint_. Take the max of first wp time and the computed time.
            addTimeToReachFirstWp( pose, velocityToFirstWaypoint_, incomingPath );

            orca::PathPlanner2dData  plannedPath = planPath( pose, incomingPath );
            orca::PathFollower2dData pathToSend = toPathFollowerData( plannedPath );

            // Work out whether we're supposed to activate immediately
            bool activateImmediately;
            activationStore_.get( activateImmediately );
            stringstream ss; ss << "MainThread: activateImmediately is " << activateImmediately;
            context_.tracer().debug(ss.str());

            // Send it off!
            cout<<"TRACE(mainthread.cpp): Sending path." << endl;
            sendPath( pathToSend, activateImmediately );

            if ( isLocalisationUncertain )
            {
                health().ok( "Generated path, but not certain about localisation.  Sent path, but will try again soon." );
                // Slow the loop down a little before trying again.
                sleep(1);
            }
            else
            {
                requestIsOutstanding = false;
                health().ok();
            }

        } // try
        catch ( const GoalPlanException &e )
        {
            stringstream ss;
            if ( e.isTemporary() )
            {
                ss << "MainThread:: Caught GoalPlanException: " << e.what() << ".  I reckon I can recover from this.";
                context_.tracer().warning( ss.str() );
                health().warning( ss.str() );

                // Slow the loop down a little before trying again.
                IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
            }
            else
            {
                ss << "MainThread:: Caught GoalPlanException: " << e.what() << ".  Looks unrecoverable, I'm giving up.";
                context_.tracer().error( ss.str() );
                health().fault( ss.str() );
                requestIsOutstanding = false;
            }
        }
        catch ( ... ) 
        {
            orcaice::catchMainLoopExceptions( health() );

            requestIsOutstanding = false;
        }
            
    } // end of big while loop
}

//////////////////////////////

void
MainThread::initNetwork()
{
    //
    // REQUIRED INTERFACES: Localise2d, Pathfollower, Pathplanner
    //

//     context_.status().initialising( subsysName(), "Connecting to Localise2d" );
    orcaice::connectToInterfaceWithTag( context_,
                                        localise2dPrx_,
                                        "Localise2d",
                                        this,
                                        subsysName() );
    // check for stop signal after retuning from multi-try
    if ( isStopping() )
        return;

//     context_.status().initialising( subsysName(), "Connecting to PathFollower2d" );
    orcaice::connectToInterfaceWithTag( context_,
                                        localNavPrx_,
                                        "PathFollower2d",
                                        this,
                                        subsysName() );
    // check for stop signal after retuning from multi-try
    if ( isStopping() )
        return;

//     context_.status().initialising( subsysName(), "Subscribing for PathFollower2d updates" );
    progressMonitor_ = new ProgressMonitor( context_ );
    progressMonitor_->subscribeWithTag( "PathFollower2d", this, subsysName() );
//     progressMonitorPtr_ = progressMonitor_;
//     progressMonitorPrx_ = orcaice::createConsumerInterface<orca::PathFollower2dConsumerPrx>( context_, progressMonitorPtr_ );
//     localNavPrx_->subscribe( progressMonitor_ );

//     context_.status().initialising( subsysName(), "Connecting to PathPlanner2d" );
    orcaice::connectToInterfaceWithTag( context_,
                                        pathplanner2dPrx_,
                                        "PathPlanner2d",
                                        this,
                                        subsysName() );
    // check for stop signal after retuning from multi-try
    if ( isStopping() )
        return;

//     context_.status().initialising( subsysName(), "Connecting to OgMap" );
    orca::OgMapPrx ogMapPrx_;
    orcaice::connectToInterfaceWithTag( context_,
                                        ogMapPrx_,
                                        "OgMap",
                                        this,
                                        subsysName() );
    // check for stop signal after retuning from multi-try
    if ( isStopping() )
        return;
    orca::OgMapData orcaOgMap = ogMapPrx_->getData();
    orcaogmap::convert( orcaOgMap, ogMap_ );

    ogLosTracer_.reset( new hydroogmap::OgLosTracer( ogMap_ ) );

    // Create consumer to receive planned paths from the path-planner.
    // (no remote calls).
    computedPathConsumer_ = new orcaifaceimpl::StoringPathPlanner2dConsumerImpl( context_ );
    
    //
    // PROVIDED INTERFACES: pathfollower2d
    //
    
    // create the proxy/buffer for incoming path
    incomingPathI_ = new PathFollower2dI( incomingPathStore_,
                                          activationStore_,
                                          localNavPrx_,
                                          context_ );
    
    Ice::ObjectPtr pathFollowerObj = incomingPathI_;

    // two possible exceptions will kill it here, that's what we want
    orcaice::createInterfaceWithTag( context_, pathFollowerObj, "PathFollower2d" );
}

void
MainThread::stopRobot()
{            
    try 
    {
        context_.tracer().debug("Stopping robot");
        orca::PathFollower2dData dummyPath;
        bool activateNow = true;
        localNavPrx_->setData( dummyPath, activateNow );
    }
    catch ( const Ice::Exception &e )
    {
        stringstream ss;
        ss << "MainThread: problem in stopRobot: " << e;
        context_.tracer().warning( ss.str() );     
        throw;
    }
}

orca::PathPlanner2dData
MainThread::planPath( const hydronavutil::Pose &pose, 
                    const orca::PathFollower2dData &coarsePath )
{
    const int timeoutMs = 1000;

    // put together a task for the pathplanner
    // add the position of the robot as the first waypoint in the path
    orca::Waypoint2d firstWp = computeFirstWaypointForPathPlanning(pose);
    orca::PathPlanner2dTask task;
    task.timeStamp  = coarsePath.timeStamp;
    task.coarsePath = coarsePath.path;
    task.coarsePath.insert( task.coarsePath.begin(), 1, firstWp );
    task.prx = computedPathConsumer_->consumerPrx();

    // send task to pathplanner
    stringstream ssSend;
    ssSend << "MainThread::"<<__func__<<": Sending task to pathplanner: " << orcaobj::toVerboseString( task );
    context_.tracer().debug(ssSend.str());
    int numJobsAheadInQueue = pathplanner2dPrx_->setTask( task );
    if ( numJobsAheadInQueue > 1 )
    {
        stringstream ss;
        ss << "MainThread::planPath(): path planner is busy, there are " << numJobsAheadInQueue << " ahead of us in the queue.";
        context_.tracer().warning( ss.str() );
    }
            
    // block until path is computed
    context_.tracer().debug("MainThread: Waiting for pathplanner's answer");
    orca::PathPlanner2dData computedPath;
    // (need a loop here so ctrlC works)
    int secWaited=0;
    while ( !isStopping() )
    {
        int ret = computedPathConsumer_->store().getNext( computedPath, timeoutMs );
        if ( ret == 0 )
            break;
        else
        {
            if ( ++secWaited > pathPlanTimeout_ )
            {
                stringstream ss;
                ss << "Did not receive a reply from the PathPlanner, after waiting " << secWaited << "s -- something must be wrong.";
                const bool isTemporary = false;
                throw( GoalPlanException( ss.str(), isTemporary ) );
            }
        }
    }
            
    // check result
    if ( computedPath.result != orca::PathOk )
    {
        stringstream ss;
        ss << "MainThread: PathPlanner could not compute.  Gave result " 
           << ifaceutil::toString( computedPath.result )<<": "<<computedPath.resultDescription;
        const bool isTemporary = true;
        throw( GoalPlanException( ss.str(), isTemporary ) );
    }

    assert( computedPath.path.size() > 0 );
    return computedPath;
}

void
MainThread::sendPath( const orca::PathFollower2dData &pathToSend, bool activateImmediately )
{
    stringstream ss;
    ss << "MainThread::"<<__func__<<": Sending path to localnav: " << orcaobj::toVerboseString(pathToSend);
    context_.tracer().debug( ss.str() );
    try {
        localNavPrx_->setData( pathToSend, activateImmediately );
    }
    catch ( Ice::Exception &e )
    {
        stringstream ss;
        ss << "MainThread:: Problem calling setData() on pathfollower2d proxy: " << e;
        const bool isTemporary = false;
        throw( GoalPlanException( ss.str(), isTemporary ) );
    }
}

void 
MainThread::tryGetLocaliseData( orca::Localise2dData &data )
{
    const unsigned int maxNumTries = 20;
    unsigned int numTries=0;
    
    while( !isStopping() )
    {
        try 
        {
            data = localise2dPrx_->getData();
            stringstream ss; ss << "MainThread: received pose: " << orcaobj::toString( data );
            context_.tracer().debug( ss.str(), 4 );
            break;
        }
        catch( orca::DataNotExistException e )
        {
            std::stringstream ss;
            ss << "Mainloop: could not fetch pose because of: " << e.what;
            context_.tracer().warning( ss.str() );
            numTries++;
            if (numTries>=maxNumTries) throw;
        }
        IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(500));
    }
}

hydronavutil::Pose
MainThread::getPose( bool &isLocalisationUncertain )
{
    // get robot pose and check what we got, may throw
    orca::Localise2dData localiseData;
    tryGetLocaliseData( localiseData );
    if ( checkForStaleLocaliseData_ )
        checkForStaleness( localiseData );

    isLocalisationUncertain = localisationIsUncertain(localiseData);
    return mlPose( localiseData );
}

bool
MainThread::needToReplan( const hydronavutil::Pose &currentPose, const orca::Waypoint2d &currentWp )
{
    bool lineOfSightToGoal = ogLosTracer_->isClearWorld( currentPose.x(),
                                                         currentPose.y(),
                                                         currentWp.target.p.x,
                                                         currentWp.target.p.y );
    //cout<<"TRACE(mainthread.cpp): lineOfSightToGoal: " << lineOfSightToGoal << endl;
    return !lineOfSightToGoal;
}

void
MainThread::replan( const hydronavutil::Pose &currentPose, const orca::Waypoint2d &currentWp )
{
    //
    // Plan to the current wp
    //
    orca::PathFollower2dData coarsePathToCurrentWp;
    coarsePathToCurrentWp.timeStamp = orcaice::getNow();
    coarsePathToCurrentWp.path.push_back( currentWp );
    orca::PathPlanner2dData pathPlanToCurrentWp = planPath( currentPose, coarsePathToCurrentWp );
    orca::PathFollower2dData pathToCurrentWp = toPathFollowerData( pathPlanToCurrentWp );

    // Now we need to construct a new path: the path to the current WP, followed
    // by the remainder of the original path
    
    // TODO: maybe the PathFollower2d interface should have functions to modify the path?
    //       Otherwise things like the 'activation time' will change now...
    //       Also, for a big path there might be a lot of copying of the old path...
    orca::PathFollower2dData newPath = pathToCurrentWp;

    // Ensure the tolerance of the next waypoint (i.e. the one we'll be hunting right now) makes sense.
    assert( newPath.path.size() > 0 );
    modifyNextWaypointTolerance( newPath.path[0], currentPose );

    // Delete the last element of newPath: it's the current waypoint, which we'll get from the oldPath.
    newPath.path.resize( newPath.path.size()-1 );
    if ( newPath.path.size() == 0 )
    {
        // The path-planner reckons we can get directly to the currentWp.
        // The path-planner probably knows best.
        context_.tracer().info( "MainThread::replan(): not replanning, since the path-planner reckons it's not necessary." );
        return;
    }
    
    //
    // Get the old path
    //
    orca::PathFollower2dData oldPath;
    int currentWpIndex;
    bool gotPath = progressMonitor_->getCurrentPath( oldPath, currentWpIndex );
    if ( !gotPath )
    {
        bool isTemporary = false;
        throw GoalPlanException( "MainThread::replan(): made a new plan, but suddenly we're not following a path anymore!  It's unlikely but possible that something changed while we were re-planning...", isTemporary );
    }
    if ( oldPath.path[currentWpIndex] != currentWp )
    {
        stringstream ss;
        ss << "MainThread::replan(): oldPath.path[currentWpIndex] = " << orcaobj::toString(oldPath.path[currentWpIndex]) << endl
           << "    but currentWp =                                " << orcaobj::toString(currentWp) << endl
           << "  These should be the same!";
        bool isTemporary = false;
        throw GoalPlanException( ss.str(), isTemporary );
    }

    // cout<<"TRACE(mainloop.cpp): old path:        " << orcaobj::toVerboseString(oldPath) << endl;
    // cout<<"TRACE(mainloop.cpp): currentWpIndex:  " << currentWpIndex << endl;
    // cout<<"TRACE(mainloop.cpp): pathToCurrentWp: " << orcaobj::toVerboseString(pathToCurrentWp) << endl;

    //
    // Shift all the times in the oldPath back because localNav is about to start a new path.
    //
    orca::PathFollower2dState pathFollowerState = localNavPrx_->getState();
    if ( !pathFollowerState.pathActivation == orca::FollowingPath )
    {
        throw GoalPlanException( "MainThread::replan(): localNavPrx_->getState says we're not following a path!", false );
    }
    const double secSinceActivation = pathFollowerState.secondsSinceActivation;
    for ( size_t i = currentWpIndex; i < oldPath.path.size(); i++ )
    {
        orca::Waypoint2d &wp = oldPath.path[i];
        if ( wp.timeTarget > secSinceActivation )
        {
            wp.timeTarget = wp.timeTarget - secSinceActivation;
        }
        else
        {
            // We're running late: aim to be there 'now'
            wp.timeTarget = 0.0;
        }
    }

    //
    // work out timeTargets for the newPath
    //

    // Calc cumulative straight-line distance along newPath
    assert( newPath.path.size() > 0 );
    std::vector<double> cumDistances( newPath.path.size() );
    cumDistances[0] = distance( currentPose, newPath.path[0] );
    for ( size_t i=1; i < newPath.path.size(); i++ )
    {
        cumDistances[i] = cumDistances[i-1] + distance( newPath.path[i-1], newPath.path[i] );
    }
    double totalDistance = cumDistances.back() + distance( newPath.path.back(), oldPath.path[currentWpIndex] );

    // Total time to allot
    double totalTime = oldPath.path[currentWpIndex].timeTarget;
    assert( totalTime >= 0.0 );

    // Spread this time out in proportion to distance
    for ( size_t i=0; i < newPath.path.size(); i++ )
    {
        double time = (cumDistances[i]/totalDistance) * totalTime;
        newPath.path[i].timeTarget = time;
    }
    
    //
    // Build complete path
    //
    cout<<"TRACE(mainloop.cpp): TODO: use 'insert'" << endl;
    // orca::Path2d::iterator remainderStart = oldPath.path.begin();
    // remainderStart += currentWpIndex;
    // newPath.path.insert( newPath.path.back(), remainderStart, oldPath.path.end() );
    for ( size_t i = currentWpIndex; i < oldPath.path.size(); i++ )
    {
        newPath.path.push_back( oldPath.path[i] );
    }
    
    // cout<<"TRACE(mainloop.cpp): secSinceActivation: " << secSinceActivation << endl;
    // cout<<"TRACE(mainloop.cpp): new path:        " << orcaobj::toVerboseString(newPath) << endl;

    //
    // Finally, send it off.
    //
    sendPath( newPath, true );
}

bool
MainThread::waitForNewPath( orca::PathFollower2dData &newPathData )
{    
    const int timeoutMs = 1000;

    while ( !isStopping() )
    {
        try {

            // Block briefly, waiting for a new path
            int ret = incomingPathStore_.getNext( newPathData, timeoutMs );
            if ( ret==0 )
            {
                // Got a new request.
                replanRequested_ = false;
                return true;
            }
            else // no new path
            {
                //
                // Don't have any new requests.  Check
                // progress on the existing path if any.
                //
                orca::Waypoint2d currentWp;
                bool havePath = progressMonitor_->getCurrentWp( currentWp );

                // cout<<"TRACE(mainloop.cpp): havePath: " << havePath << endl;

                if ( havePath )
                {
                    // get robot pose
                    bool isLocalisationUncertain;
                    hydronavutil::Pose pose = getPose(isLocalisationUncertain);

                    if ( enableReplan_ && needToReplan( pose, currentWp ) )
                    {
                        //cout<<"TRACE(mainthread.cpp): Need to replan!" << endl;
                        if ( !replanRequested_ )
                        {
                            //cout<<"TRACE(mainthread.cpp): Starting timer..." << endl;

                            context_.tracer().info( "MainThread: current wp is not visible -- requesting replan." );
                            replanRequested_ = true;
                            replanRequestTimer_.restart();
                        }
                        else
                        {
                            // Replan previously requested
                            //cout<<"TRACE(mainthread.cpp): replan request timer: " << replanRequestTimer_.elapsedSec() << "s" << endl;
                            if ( replanRequestTimer_.elapsedSec() >= requiredReplanRequestTime_ )
                            {
                                // Time has elapsed

                                if ( isLocalisationUncertain )
                                {
                                    stringstream ss;
                                    ss << "MainThread: need to replan, but localisation is too uncertain!";
                                    context_.tracer().warning( ss.str() );
                                    health().warning( ss.str() );
                                    continue;
                                }
                                else
                                {
                                    context_.tracer().info( "MainThread: current wp is not visible -- replanning." );
                                    replan( pose, currentWp );
                                    replanRequested_ = false;
                                }
                            }
                        }
                    }
                    else
                    {
                        //cout<<"TRACE(mainthread.cpp): Don't need to replan, stopping timer." << endl;
                        replanRequested_ = false;
                    }
                }
                else
                {
                    replanRequested_ = false;
                }

                // Let the world know we're alive.
                health().ok();
            }
        }
        catch ( ... ) {
            orcaice::catchExceptionsWithStatusAndSleep( "waiting for new path", health() );
        }   
    }
    return false;
}

}
