/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#include <orcaice/orcaice.h>
#include <orcamisc/cov2d.h>
#include "mainloop.h"
#include "pathfollower2dI.h"
#include "localise2dconsumerI.h"
#include "pathplanner2dconsumerI.h"

using namespace std;
using namespace orca;
using namespace goalplanner;

// ======== NON-MEMBER FUNCTION ======================
void computeFirstWaypoint( const Localise2dData &localiseData, Waypoint2d &wp)
{
    
    cout << "TRACE(mainloop.cpp): Localised frame is: " 
            << localiseData.hypotheses[0].mean.p.x << " " 
            << localiseData.hypotheses[0].mean.p.y << " " 
            << localiseData.hypotheses[0].mean.o << endl; 
    
    cout << "TRACE(mainloop.cpp): Localised covariance values are: " 
            << localiseData.hypotheses[0].cov.xx << " " 
            << localiseData.hypotheses[0].cov.xy << " " 
            << localiseData.hypotheses[0].cov.yy << " " 
            << localiseData.hypotheses[0].cov.tt << " " 
            << endl;

    // take the mean of the hypotheses as the waypoint
    wp.target = localiseData.hypotheses[0].mean;
    // add heading tolerance from uncertainty
    wp.headingTolerance = sqrt(localiseData.hypotheses[0].cov.tt);
    
    // hardcode approach values
    wp.maxApproachSpeed = 2e+6;
    wp.maxApproachTurnrate = (float)DEG2RAD(2e+6); 
    
    // add distance tolerance
    double a, b, th;
    orcamisc::Cov2d cov(localiseData.hypotheses[0].cov.xx, 
                        localiseData.hypotheses[0].cov.xy, 
                        localiseData.hypotheses[0].cov.yy);
    cov.ellipse( a, b, th );
    cout << "TRACE(mainloop.cpp): a,b,th: " << a << "," << b << "," << th << endl;
    
    // take the larger of the two and multiply to get 3 sigma (cov.ellipse gives us 2 sigma)
    wp.distanceTolerance = a > b ? a/2.0*3.0 : b/2.0*3.0;
}
// =======================================================

MainLoop::MainLoop( const orcaice::Context & context )
    : incomingPathI_(0),
      context_(context)
{
    initNetwork();
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
        try
        {
            orcaice::connectToInterfaceWithTag<orca::Localise2dPrx>( context_, localise2dPrx_, "Localise2d" );
            break;
        }
        catch ( const orcaice::NetworkException & )
        {
	    stringstream ss;
	    ss << "Localise2d: failed to connect to remote object. Will try again after 3 seconds.";
            context_.tracer()->error( ss.str() );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
    }
    
    //create a callback to receive localisation data
    Ice::ObjectPtr consumer = new Localise2dConsumerI(localiseDataBuffer_);
    localiseConsumerPrx_ =
            orcaice::createConsumerInterface<orca::Localise2dConsumerPrx>( context_, consumer );

    while ( isActive() )
    {
        try
        {
            localise2dPrx_->subscribe( localiseConsumerPrx_ );
            break;
        }
        catch ( const orca::SubscriptionFailedException & )
        {
            context_.tracer()->error( "failed to subscribe for localise2d data updates. Will try again after 3 seconds." );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
    }
    
    
    while( isActive() )
    {
        try
        {
            orcaice::connectToInterfaceWithTag<orca::PathFollower2dPrx>( context_, localNavPrx_, "PathFollower2d" );
            break;
        }
        catch ( const orcaice::NetworkException & )
        {
            context_.tracer()->error( "failed to connect to remote pathfollower2d object. Will try again after 3 seconds." );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
    }
    
    while( isActive() )
    {
        try
        {
            orcaice::connectToInterfaceWithTag<orca::PathPlanner2dPrx>( context_, pathplanner2dPrx_, "PathPlanner2d" );
            break;
        }
        catch ( const orcaice::NetworkException & )
        {
            context_.tracer()->error( "failed to connect to remote pathplanner2d object. Will try again after 3 seconds." );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
    }
    
    Ice::ObjectPtr pathConsumer = new PathPlanner2dConsumerI( computedPathBuffer_ );
    taskPrx_ = orcaice::createConsumerInterface<PathPlanner2dConsumerPrx>( context_, pathConsumer );

    //
    // PROVIDED INTERFACES: pathfollower2d
    //
    
    // create the proxy/buffer for incoming path

    IceStorm::TopicPrx topicPrx = orcaice::connectToTopicWithTag<PathFollower2dConsumerPrx>
            ( context_, pathPublisher_, "PathFollower2d" );
    
    incomingPathI_ = new PathFollower2dI( incomingPathBuffer_,
                                          activationBuffer_,
                                          localise2dExceptionBuffer_,
                                          localNavPrx_,
                                          topicPrx );
    
    Ice::ObjectPtr pathFollowerObj = incomingPathI_;

    // two possible exceptions will kill it here, that's what we want
    orcaice::createInterfaceWithTag( context_, pathFollowerObj, "PathFollower2d" );
}

void 
MainLoop::run()
{
    
    PathFollower2dData incomingPath;
    Localise2dData localiseData;
    
    PathPlanner2dTask task;
    PathPlanner2dConsumerPrx callbackPrx;
    PathPlanner2dData computedPath;
    
    Waypoint2d wp;
    orcaice::setInit( wp );
    
    // main loop
    while ( isActive() )
    {
        
        try
        {
            // wait for a goal path
            context_.tracer()->info("Waiting for a goal path");
            while( isActive() )
            {
                // get wp index from localnav and publish to the world while we're waiting for a new goal
                pathPublisher_->setWaypointIndex( localNavPrx_->getWaypointIndex() );
                int ret = incomingPathBuffer_.getNext( incomingPath, 500 );
                if (ret==0) break;
            }
            
            // wait for a valid localisation
            context_.tracer()->info("Waiting for single hypothesis localisation");
            while( isActive() )
            {
                int ret = localiseDataBuffer_.getNext( localiseData, 1000 );
                // store in special buffer, so PathFollower2dI can access it for throwing exceptions
                localise2dExceptionBuffer_.set( localiseData );
                if (ret==0)
                {
                    if ( localiseData.hypotheses.size() == 1 ) break;
                    
                    context_.tracer()->warning("More than one localisation hypotheses. Can't handle this. Waiting for single hypothesis...");
                }
            }
                
            // we're guaranteed to have only 1 hypothesis
            // compute tolerances for the first waypoint based on where I think I am
            computeFirstWaypoint(localiseData, wp);
            
            // put together a task for the pathplanner
            // add the position of the robot as the first waypoint in the path
            incomingPath.path.insert( incomingPath.path.begin(), 1, wp );
            task.coarsePath = incomingPath.path;
            task.prx = taskPrx_;
            
            // send task to pathplanner
            stringstream ss;
            ss << "Sending task to pathplanner: " << orcaice::toVerboseString( task );
            context_.tracer()->debug(ss.str());
            try {
                pathplanner2dPrx_->setTask( task );
            }
            catch (orca::RequiredInterfaceFailedException &e)
            {
                stringstream ss;
                ss << e.what;
                context_.tracer()->warning( ss.str() ); 
                throw;   
            }
            catch (orca::BusyException &e)
            {
                stringstream ss;
                ss << e.what;
                context_.tracer()->warning( ss.str() );      
                throw;
            }
            
            // block until path is computed
            context_.tracer()->debug("Waiting for pathplanner's answer");
            while( isActive() )
            {
                int ret = computedPathBuffer_.getNext( computedPath, 1000 );
                if (ret==0) break;
            }
            
            // check result
            if ( computedPath.result!= PathOk )
            {
                context_.tracer()->warning("Pathplanner could not compute path. Give me another goal");
            }
            else
            {
                // send out result to localnav, assemble packet first
                PathFollower2dData outgoingPath;
                outgoingPath.path = computedPath.path;
                context_.tracer()->debug("Sending out the resulting path to localnav.");
                try {
                    // get what's currently in the activation pipe
                    bool activation=0;
                    while( isActive() )
                    {
                        int ret = activationBuffer_.getNext( activation, 1000 );
                        if (ret==0) break;
                    }
                    stringstream ss; ss << "Activation is " << activation;
                    context_.tracer()->debug(ss.str());
                    localNavPrx_->setData( outgoingPath, activation );

                    // consumers should see what I send to localnav for proxy functionality
                    pathPublisher_->setData( outgoingPath );
                    pathPublisher_->setWaypointIndex( localNavPrx_->getWaypointIndex() );
                }
                catch ( Ice::NotRegisteredException & )
                {
                    stringstream ss;
                    ss << "Problem setting data on pathfollower2d proxy";
                    context_.tracer()->warning( ss.str() );     
                    throw;
                }
    
            }
    
        } // try
        catch ( const orca::OrcaException & e )
        {
            stringstream ss;
            ss << "unexpected (remote?) orca exception: " << e << ": " << e.what;
            context_.tracer()->error( ss.str() );
        }
        catch ( const orcaice::Exception & e )
        {
            stringstream ss;
            ss << "unexpected (local?) orcaice exception: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        catch ( const Ice::Exception & e )
        {
            stringstream ss;
            ss << "unexpected Ice exception: " << e;
            context_.tracer()->error( ss.str() );
        }
        catch ( const std::exception & e )
        {
            // once caught this beast in here, don't know who threw it 'St9bad_alloc'
            stringstream ss;
            ss << "unexpected std exception: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        catch ( ... )
        {
            context_.tracer()->error( "unexpected exception from somewhere.");
        }
            
    } // end of big while loop
    
    cout << "TRACE(mainloop.cpp): End of run() now..." << endl;
    
    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
}


