/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include <iostream>
#include <cmath>
#include <orcaice/orcaice.h>

#include "mainloop.h"
#include "pathmaintainer.h"
#include "pathfollower2dI.h"
#include "testsim/simulator.h"

using namespace std;
using namespace orca;
using namespace orcaice;

namespace localnav {

MainLoop::MainLoop( LocalNavManager                                &localNavManager,
                    orcaice::PtrProxy<orca::RangeScanner2dDataPtr> &obsProxy,
                    orcaice::Proxy<orca::Localise2dData>           &locProxy,
                    orcaice::Proxy<orca::Position2dData>           &odomProxy,
                    PathFollower2dI                                &pathFollowerInterface,
                    orca::Platform2dPrx                            &platform2dPrx,
                    PathMaintainer                                 &pathMaintainer,
                    orca::PathFollower2dConsumerPrx                &pathPublisher,
                    const orcaice::Context                         &context )
    : localNavManager_(localNavManager),
      obsProxy_(obsProxy),
      locProxy_(locProxy),
      odomProxy_(odomProxy),
      pathFollowerInterface_(pathFollowerInterface),
      platform2dPrx_(platform2dPrx),
      testMode_(false),
      pathMaintainer_(pathMaintainer),
      pathPublisher_(pathPublisher),
      heartbeater_(context),
      context_(context)
{
}

MainLoop::MainLoop( LocalNavManager                                &localNavManager,
                    orcaice::PtrProxy<orca::RangeScanner2dDataPtr> &obsProxy,
                    orcaice::Proxy<orca::Localise2dData>           &locProxy,
                    orcaice::Proxy<orca::Position2dData>           &odomProxy,
                    PathFollower2dI                                &pathFollowerInterface,
                    Simulator                                      &testSimulator,
                    PathMaintainer                                 &pathMaintainer,
                    orca::PathFollower2dConsumerPrx                &pathPublisher,
                    const orcaice::Context                         &context )
    : localNavManager_(localNavManager),
      obsProxy_(obsProxy),
      locProxy_(locProxy),
      odomProxy_(odomProxy),
      pathFollowerInterface_(pathFollowerInterface),
      testSimulator_(&testSimulator),
      testMode_(true),
      pathMaintainer_(pathMaintainer),
      pathPublisher_(pathPublisher),
      heartbeater_(context),
      context_(context)
{
}

MainLoop::~MainLoop()
{
}

void
MainLoop::ensureProxiesNotEmpty()
{
    // Ensure that there's something in our proxys
    while ( isActive() )
    {
        bool empty = 
            obsProxy_.isEmpty() ||
            locProxy_.isEmpty() || 
            odomProxy_.isEmpty();

        if ( empty )
        {
            context_.tracer()->warning( "Still waiting for intial data to arrive..." );
            sleep(1);
        }
        else
            return;
            

//         int ret = 0;
//         ret = ret || obsProxy_.getNext(  rangeData_,    1000 );
//         ret = ret || locProxy_.getNext(  localiseData_, 1000 );
//         ret = ret || odomProxy_.getNext( odomData_,     1000 );

//         if ( ret == 0 )
//             break;
//         else 
//         {
//             context_.tracer()->warning( "Still waiting for intial data to arrive..." );
//         }
    }
}

void 
MainLoop::getStopCommand( orca::Velocity2dCommand& cmd )
{
    cmd.motion.v.x = 0.0;
    cmd.motion.v.y = 0.0;
    cmd.motion.w   = 0.0;
}

void
MainLoop::initInterfaces()
{
    while ( isActive() )
    {
        try {
            pathFollowerInterface_.initInterface();
            return;
        }
        catch ( orcaice::Exception &e )
        {
            stringstream ss;
            ss << "MainLoop: Failed to initialise PathFollower2d interface: " << e.what();
            context_.tracer()->warning( ss.str() );
            if ( testMode_ )
            {
                context_.tracer()->warning( "Continuing regardless..." );
                return;
            }
        }
    }
}

void
MainLoop::run()
{
    const int TIMEOUT_MS = 1000;

    initInterfaces();
    ensureProxiesNotEmpty();

    try 
    {
        while ( isActive() )
        {
            //cout<<"============================================="<<endl;

            // The rangeScanner provides the 'clock' which is the trigger for this loop
            int sensorRet = obsProxy_.getNext( rangeData_, TIMEOUT_MS );

            // Before we do anything, check whether we're enabled.
            if ( !pathFollowerInterface_.localIsEnabled() )
            {
                context_.tracer()->debug( "Doing nothing because disabled" );
                continue;
            }

            if ( sensorRet != 0 )
            {
                stringstream ss;
                ss << "Timeout waiting for range data: no data for " << TIMEOUT_MS << "ms.  Stopping.";
                context_.tracer()->error( ss.str() );
                getStopCommand( velocityCmd_ );
            }
            else
            {
                locProxy_.get( localiseData_ );
                odomProxy_.get( odomData_ );

                const double THRESHOLD = 1.0; // seconds
                if ( areTimestampsDodgy( rangeData_, localiseData_, odomData_, THRESHOLD ) )
                {
                    stringstream ss;
                    ss << "Timestamps are more than "<<THRESHOLD<<"sec apart: " << endl
                       << "\t rangeData:    " << orcaice::toString(rangeData_->timeStamp) << endl
                       << "\t localiseData: " << orcaice::toString(localiseData_.timeStamp) << endl
                       << "\t odomData:     " << orcaice::toString(odomData_.timeStamp) << endl
                       << "Maybe something is wrong: Stopping.";
                    context_.tracer()->error( ss.str() );
                    getStopCommand( velocityCmd_ );
                }
                else
                {
                    velocityCmd_.timeStamp = rangeData_->timeStamp;

//                     cout<<"TRACE(mainloop.cpp): localNavManager_.getCommand:"<<endl
//                         <<"    localiseData: " << orcaice::toString(localiseData_) << endl
//                         <<"    odomData: " << orcaice::toString(odomData_) << endl;

                    localNavManager_.getCommand( rangeData_,
                                                 localiseData_,
                                                 odomData_,
                                                 velocityCmd_ );
                }
            }
            
            // Send the command to the platform
            try {
                if ( testMode_ )
                    testSimulator_->setCommand( velocityCmd_ );
                else
                    platform2dPrx_->setCommand( velocityCmd_ );
            }
            catch ( orca::HardwareFailedException &e )
            {
                stringstream ss;
                ss << e.what;
                context_.tracer()->warning( ss.str() );
            }
            catch ( const Ice::Exception &e )
            {
                stringstream ss;
                ss << "While giving command to platform: " << e;
                context_.tracer()->warning( ss.str() );
            }
            
            checkWithOutsideWorld( pathMaintainer_ );
            maybeSendHeartbeat();
        }
    }
    catch ( Ice::CommunicatorDestroyedException &e )
    {
        // This is OK: it means that the communicator shut down (eg via Ctrl-C)
        // somewhere in mainLoop.
        //
        // Could probably handle it better for an Application by stopping the component on Ctrl-C
        // before shutting down communicator.
    }
    catch ( std::exception &e )
    {
        std::stringstream ss;
        ss << "mainloop.cpp: caught std::exception: " << e.what();
        context_.tracer()->error( ss.str() );
    }
    catch ( std::string &e )
    {
        std::stringstream ss;
        ss << "mainloop.cpp: caught std::string: " << e;
        context_.tracer()->error( ss.str() );
    }
    catch ( char *e )
    {
        std::stringstream ss;
        ss << "mainloop.cpp: caught char*: " << e;
        context_.tracer()->error( ss.str() );
    }
    catch ( Ice::Exception &e )
    {
        std::stringstream ss;
        ss << "ERROR(mainloop.cpp): Caught unexpected exception: " << e;
        context_.tracer()->error( ss.str() );
    }
    catch ( ... )
    {
        std::stringstream ss;
        ss << "ERROR(mainloop.cpp): Caught unexpected unknown exception.";
        context_.tracer()->error( ss.str() );
    }
    
    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
    
    cout<<"TRACE(mainloop.cpp): Dropping out from run()" << endl;
}

// Tobi: why do you pass a parameter here and then use the member variable?
void
MainLoop::checkWithOutsideWorld( PathMaintainer &pathMaintainer )
{
    // (1): world->localnav: Have we received a new path?
    pathMaintainer_.checkForNewPath( pathPublisher_ );

    // (2): world<-localnav: Have we modified our wp index?
    pathMaintainer_.checkForWpIndexChange( pathPublisher_ );
}

bool
MainLoop::areTimestampsDodgy( const orca::RangeScanner2dDataPtr &rangeData, 
                              const orca::Localise2dData&       localiseData, 
                              const orca::Position2dData&       odomData,
                              double                           threshold )
{
    if ( fabs( orcaice::timeDiffAsDouble( rangeData->timeStamp, localiseData.timeStamp ) ) >= threshold )
        return true;
    if ( fabs( orcaice::timeDiffAsDouble( rangeData->timeStamp, odomData.timeStamp ) ) >= threshold )
        return true;

    return false;
}

void
MainLoop::maybeSendHeartbeat()
{
    if ( heartbeater_.isHeartbeatTime() )
    {
        // construct the heartbeat message
        stringstream ss;
        ss << "MainLoop is running." << endl
           << localNavManager_.getHeartbeatMessage();
    }
}

}
