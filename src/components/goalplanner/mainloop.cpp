/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#include <orcaice/orcaice.h>
#include <orcaobj/miscutils.h>
#include <orcaobj/stringutils.h>
#include <orcaobj/timeutils.h>

#include "pathfollower2dI.h"
#include "localise2dconsumerI.h"
#include "pathplanner2dconsumerI.h"

#include "mainloop.h"


using namespace std;
using namespace orca;
using namespace goalplanner;

namespace {
    const char *SUBSYSTEM = "mainloop";

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

    // Computes waypoint to start from
    // This is for the pathplanner's purposes, not the pathfollower.
    // So we don't care too much about tolerances and speeds.
    void computeFirstWaypointForPathPlanning( const orcanavutil::Pose &pose, Waypoint2d &wp )
    {
        wp.target.p.x = pose.x();
        wp.target.p.y = pose.y();
        wp.target.o   = pose.theta();

        // add bogus tolerances and speeds
        wp.distanceTolerance = (Ice::Float)0.1;
        wp.headingTolerance  = (Ice::Float)(M_PI/2.0);
        wp.timeTarget.seconds  = 0;
        wp.timeTarget.useconds = 0;
        wp.maxApproachSpeed    = 2e+6;
        wp.maxApproachTurnrate = (float)DEG2RAD(2e+6); 
    }

    orcanavutil::Pose mlPose( const orca::Localise2dData &localiseData )
    {
        const orca::Pose2dHypothesis &mlHyp = orcaice::mlHypothesis( localiseData );
        return orcanavutil::Pose( mlHyp.mean.p.x, mlHyp.mean.p.y, mlHyp.mean.o );
    }

    bool localisationIsUncertain( const orca::Localise2dData &localiseData )
    {
        if ( localiseData.hypotheses.size() > 1 )
            return false;
        
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
}

MainLoop::MainLoop( const orcaice::Context & context )
    : incomingPathI_(0),
      context_(context)
{
    context_.status()->setMaxHeartbeatInterval( SUBSYSTEM, 10.0 );
    context_.status()->initialising( SUBSYSTEM );
    initNetwork();

    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag()+".Config.";
    pathPlanTimeout_ = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"PathPlanTimeout", 10.0 );
    velocityToFirstWaypoint_ = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"VelocityToFirstWaypoint", 1.0 );
}

MainLoop::~MainLoop()
{
}

void
MainLoop::initNetwork()
{
    //
    // REQUIRED INTERFACES: Localise2d, Pathfollower, Pathplanner
    //

    while( isActive() )
    {
        context_.status()->initialising( SUBSYSTEM, "Connecting to Localise2d" );
        try
        {
            orcaice::connectToInterfaceWithTag<orca::Localise2dPrx>( context_, localise2dPrx_, "Localise2d" );
            break;
        }
        catch ( const Ice::Exception &e )
        {
            stringstream ss;
            ss << "Localise2d: failed to connect to Localise2d: " << e << ". Will try again after 3 seconds.";
            context_.tracer()->error( ss.str() );
        }
        catch ( const std::exception &e )
        {
            stringstream ss;
            ss << "Localise2d: failed to connect to Localise2d: " << e.what() << ". Will try again after 3 seconds.";
            context_.tracer()->error( ss.str() );
        }
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
    }

    // create a callback to receive localisation data
    // TODO: AlexB: should we really be subscribing for localise data?  Probably it's sufficient to just poll as necessary??
    Ice::ObjectPtr consumer = new Localise2dConsumerI(localiseDataBuffer_);
    localiseConsumerPrx_ =
            orcaice::createConsumerInterface<orca::Localise2dConsumerPrx>( context_, consumer );

    while ( isActive() )
    {
        context_.status()->initialising( SUBSYSTEM, "Subscribing to Localise2d" );
        try
        {
            localise2dPrx_->subscribe( localiseConsumerPrx_ );
            break;
        }
        catch ( const Ice::Exception &e )
        {
            stringstream ss;
            ss << "MainLoop: failed to subscribe for localise2d data updates: " << e << ". Will try again after 3 seconds.";
            context_.tracer()->error( ss.str() );
        }
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
    }    
    
    while( isActive() )
    {
        context_.status()->initialising( SUBSYSTEM, "Waiting for Localise2d data" );
        orca::Localise2dData data;
        int ret = localiseDataBuffer_.getNext( data, 1000 );
        if ( ret == 0 )
            break;
    }
    
    while( isActive() )
    {
        context_.status()->initialising( SUBSYSTEM, "Connecting to PathFollower2d" );
        try
        {
            orcaice::connectToInterfaceWithTag<orca::PathFollower2dPrx>( context_, localNavPrx_, "PathFollower2d" );
            break;
        }
        catch ( const Ice::Exception &e )
        {
            stringstream ss;
            ss << "failed to connect to remote pathfollower2d object: " << e << ". Will try again after 3 seconds.";
            context_.tracer()->error( ss.str() );
        }
        catch ( const std::exception &e )
        {
            stringstream ss;
            ss << "failed to connect to remote pathfollower2d object: " << e.what() << ". Will try again after 3 seconds.";
            context_.tracer()->error( ss.str() );
        }
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
    }
    
    while( isActive() )
    {
        context_.status()->initialising( SUBSYSTEM, "Connecting to PathPlanner2d" );
        try
        {
            orcaice::connectToInterfaceWithTag<orca::PathPlanner2dPrx>( context_, pathplanner2dPrx_, "PathPlanner2d" );
            break;
        }
        catch ( const Ice::Exception &e )
        {
            stringstream ss;
            ss << "failed to connect to remote pathplanner2d object: " << e << ".  Will try again after 3 seconds.";
            context_.tracer()->error( ss.str() );
        }
        catch ( const std::exception &e )
        {
            stringstream ss;
            ss << "failed to connect to remote pathplanner2d object: " << e.what() << ".  Will try again after 3 seconds.";
            context_.tracer()->error( ss.str() );
        }
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
    }
    
    Ice::ObjectPtr pathConsumer = new PathPlanner2dConsumerI( computedPathBuffer_ );
    taskPrx_ = orcaice::createConsumerInterface<PathPlanner2dConsumerPrx>( context_, pathConsumer );

    //
    // PROVIDED INTERFACES: pathfollower2d
    //
    
    // create the proxy/buffer for incoming path
    incomingPathI_ = new PathFollower2dI( incomingPathBuffer_,
                                          activationBuffer_,
                                          localNavPrx_ );
    
    Ice::ObjectPtr pathFollowerObj = incomingPathI_;

    // two possible exceptions will kill it here, that's what we want
    orcaice::createInterfaceWithTag( context_, pathFollowerObj, "PathFollower2d" );
}

void MainLoop::stopRobot()
{            
    try 
    {
        context_.tracer()->debug("Stopping robot");
        PathFollower2dData dummyPath;
        bool activateNow = true;
        localNavPrx_->setData( dummyPath, activateNow );
    }
    catch ( const Ice::Exception &e )
    {
        stringstream ss;
        ss << "MainLoop: problem in stopRobot: " << e;
        context_.tracer()->warning( ss.str() );     
        throw;
    }
}

void
MainLoop::adjustTimes( const orcanavutil::Pose &pose,
                       orca::PathFollower2dData &incomingPath )
{  
    // compute time to reach 1st wp based on straight-line distance: timeDist
    assert( incomingPath.path.size()>0 );
    Waypoint2d &firstWp = incomingPath.path[0];
    double dX = pose.x()-firstWp.target.p.x;
    double dY = pose.y()-firstWp.target.p.y;
    double dist = sqrt( dX*dX + dY*dY );
    double timeDist = dist/velocityToFirstWaypoint_;
    
    // time to reach 1st waypoint according to incoming path: timeIn
    double timeIn = orcaice::timeAsDouble( firstWp.timeTarget );
    
    // compute difference between the two
    double timeOffset =  timeDist - timeIn;
    
    // nothing to change if timeIn is bigger than timeDist
    if (timeOffset<0.0) return; 
    
    // otherwise: add the offset to all waypoints in the path
    for (unsigned int i=0; i<incomingPath.path.size(); i++)
    {
        Waypoint2d &wp = incomingPath.path[i];
        wp.timeTarget = orcaice::toOrcaTime( orcaice::timeAsDouble(wp.timeTarget) + timeOffset );
    }
}

void 
MainLoop::computeAndSendPath( const orcanavutil::Pose &pose, 
                              const orca::PathFollower2dData &incomingPath )
{   
    // put together a task for the pathplanner
    // add the position of the robot as the first waypoint in the path
    Waypoint2d wp;
    computeFirstWaypointForPathPlanning(pose, wp);
    PathPlanner2dTask task;
    task.timeStamp  = incomingPath.timeStamp;
    task.coarsePath = incomingPath.path;
    task.coarsePath.insert( task.coarsePath.begin(), 1, wp );
    task.prx = taskPrx_;

    // send task to pathplanner
    stringstream ssSend;
    ssSend << "MainLoop: Sending task to pathplanner: " << orcaice::toVerboseString( task );
    context_.tracer()->debug(ssSend.str());
    pathplanner2dPrx_->setTask( task );
            
    // block until path is computed
    context_.tracer()->debug("MainLoop: Waiting for pathplanner's answer");
    PathPlanner2dData computedPath;
    // (need a loop here so ctrlC works)
    int secWaited=0;
    while ( isActive() )
    {
        int ret = computedPathBuffer_.getNext( computedPath, 1000 );
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
    if ( computedPath.result!= PathOk )
    {
        stringstream ss;
        ss << "MainLoop: PathPlanner could not compute.  Gave result " << orcaice::toString( computedPath.result )<<": "<<computedPath.resultDescription;
        const bool isTemporary = true;
        throw( GoalPlanException( ss.str(), isTemporary ) );
    }

    assert( computedPath.path.size() > 0 );

    // send out result to localnav, assemble packet first
    PathFollower2dData outgoingPath;
    outgoingPath.timeStamp = computedPath.timeStamp;
    outgoingPath.path = computedPath.path;
    // get rid of first waypoint, it's the robot's location which is not needed
    vector<orca::Waypoint2d>::iterator it = outgoingPath.path.begin();
    outgoingPath.path.erase(it);
                
    // Work out whether we're supposed to activate immediately
    bool activation=0;
    activationBuffer_.get( activation );
    stringstream ss; ss << "Activation is " << activation;
    context_.tracer()->debug(ss.str());

    // Send result to localNav
    context_.tracer()->debug("MainLoop: Sending out the resulting path to localnav.");
    try {
        localNavPrx_->setData( outgoingPath, activation );
    }
    catch ( Ice::Exception &e )
    {
        stringstream ss;
        ss << "MainLoop:: Problem calling setData() on pathfollower2d proxy: " << e;
        const bool isTemporary = false;
        throw( GoalPlanException( ss.str(), isTemporary ) );
    }
}

void 
MainLoop::run()
{
    PathFollower2dData incomingPath;
    bool requestIsOutstanding = false;

    context_.status()->setMaxHeartbeatInterval( SUBSYSTEM, 3.0 );

    // main loop
    while ( isActive() )
    {
        try
        {
            // If we have an outstanding request, service it now
            // (but check to see if we should over-write with a new one)
            // Otherwise, wait for a new request.
            if ( requestIsOutstanding )
            {
                if ( !incomingPathBuffer_.isEmpty() )
                {
                    // Overwrite the unserviced request with the new one.
                    incomingPathBuffer_.get( incomingPath );
                }
            }
            else
            {
                context_.tracer()->info("Waiting for a goal path");
                while( isActive() )
                {
                    int ret = incomingPathBuffer_.getNext( incomingPath, 1000 );
                    if (ret==0)
                    {
                        requestIsOutstanding = true;
                        break;
                    }
                    else
                    {
                        // Keep waiting, but let the world know we're alive
                        context_.status()->ok( SUBSYSTEM );
                    }
                }
            }

            stringstream ssPath;
            ssPath << "MainLoop: Requested path: " << endl << orcaice::toVerboseString(incomingPath);
            context_.tracer()->debug( ssPath.str() );

            // special case 'stop': we received an empty path
            if (incomingPath.path.size()==0) 
            {
                stopRobot();
                context_.status()->ok( SUBSYSTEM );
                requestIsOutstanding = false;
                continue;
            }

            // TODO: what if localiseData is stale?
            orca::Localise2dData localiseData;
            localiseDataBuffer_.get( localiseData );
            const double AGE_FOR_STALE = 3; // seconds
            if ( ageOf( localiseData.timeStamp ) > AGE_FOR_STALE )
            {
                stringstream ss;
                ss << "MainLoop: LocaliseData is stale: age is " << ageOf( localiseData.timeStamp ) << " sec";
                context_.tracer()->warning( ss.str() );
                context_.status()->warning( SUBSYSTEM, ss.str() );
            }
                        
            // Adjust timing: work out how long it takes to the first waypoint based on straight-line distance 
            // and configured velocityToFirstWaypoint_. Take the max of first wp time and the computed time.
            const orcanavutil::Pose &pose = mlPose(localiseData);
            adjustTimes( pose, incomingPath );
            
            computeAndSendPath( pose, incomingPath );

            // We've only serviced the request properly if we're certain of our position.
            // If we're not certain, keep trying till we become certain.
            if ( localisationIsUncertain(localiseData) )
            {
                context_.tracer()->info( "MainLoop: path was sent but localisation was uncertain...  We'll try again soon." );
            }
            else
            {
                context_.tracer()->info( "MainLoop: localisation was OK -- request no longer outstanding." );
                requestIsOutstanding = false;
            }

            context_.status()->ok( SUBSYSTEM );

        } // try
        catch ( const GoalPlanException &e )
        {
            stringstream ss;
            if ( e.isTemporary() )
            {
                ss << "MainLoop:: Caught GoalPlanException: " << e.what() << ".  I reckon I can recover from this.";
                context_.tracer()->warning( ss.str() );
                context_.status()->warning( SUBSYSTEM, ss.str() );

                // Slow the loop down a little before trying again.
                IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
            }
            else
            {
                ss << "MainLoop:: Caught GoalPlanException: " << e.what() << ".  Looks unrecoverable, I'm giving up.";
                context_.tracer()->error( ss.str() );
                context_.status()->fault( SUBSYSTEM, ss.str() );
                requestIsOutstanding = false;
            }
        }
        catch ( const Ice::Exception & e )
        {
            stringstream ss;
            ss << "MainLoop:: Caught exception: " << e;
            context_.tracer()->error( ss.str() );
            context_.status()->fault( SUBSYSTEM, ss.str() );
            requestIsOutstanding = false;
        }
        catch ( const std::exception & e )
        {
            stringstream ss;
            ss << "MainLoop: Caught exception: " << e.what();
            context_.tracer()->error( ss.str() );
            context_.status()->fault( SUBSYSTEM, ss.str() );
            requestIsOutstanding = false;
        }
        catch ( ... )
        {
            context_.tracer()->error( "MainLoop: caught unknown unexpected exception.");
            context_.status()->fault( SUBSYSTEM, "MainLoop: caught unknown unexpected exception.");
            requestIsOutstanding = false;
        }
            
    } // end of big while loop
    
    cout << "TRACE(mainloop.cpp): End of run() now..." << endl;
    
    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
}


