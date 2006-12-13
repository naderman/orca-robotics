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

using namespace std;
using namespace orca;
using namespace orcaice;

namespace localnav {

MainLoop::MainLoop( LocalNavManager                               &localNavManager,
                    orcaice::PtrBuffer<orca::RangeScanner2dDataPtr> &obsBuffer,
                    orcaice::PtrBuffer<orca::Localise2dDataPtr>   &locBuffer,
                    orcaice::PtrBuffer<orca::Position2dDataPtr>   &odomBuffer,
                    orcaice::Proxy<bool>                          &enabledPipe,
                    orca::Platform2dPrx                           &platform2dPrx,
                    PathMaintainer                                &pathMaintainer,
                    orca::PathFollower2dConsumerPrx               &pathPublisher,
                    orcaice::Context                               context )
    : localNavManager_(localNavManager),
      obsBuffer_(obsBuffer),
      locBuffer_(locBuffer),
      odomBuffer_(odomBuffer),
      enabledPipe_(enabledPipe),
      platform2dPrx_(platform2dPrx),
      pathMaintainer_(pathMaintainer),
      pathPublisher_(pathPublisher),
      heartbeater_(context),
      context_(context)
{
    localiseData_ = new orca::Localise2dData;
    odomData_     = new orca::Position2dData;
    rangeData_    = new orca::RangeScanner2dData;
    velocityCmd_  = new orca::Velocity2dCommand;
}

MainLoop::~MainLoop()
{
}

void
MainLoop::ensureBuffersNotEmpty()
{
    // Ensure that there's something in our buffers
    while ( isActive() )
    {
        int ret = 0;
        ret = ret || obsBuffer_.getNext(  rangeData_,    1000 );
        ret = ret || locBuffer_.getNext(  localiseData_, 1000 );
        ret = ret || odomBuffer_.getNext( odomData_,     1000 );

        if ( ret == 0 )
            break;
        else 
        {
            context_.tracer()->warning( "Still waiting for intial data to arrive..." );
        }
    }
}

void 
MainLoop::getStopCommand( orca::Velocity2dCommandPtr cmd )
{
    cmd->motion.v.x = 0.0;
    cmd->motion.v.y = 0.0;
    cmd->motion.w   = 0.0;
}

void
MainLoop::run()
{
    const int TIMEOUT_MS = 1000;

    ensureBuffersNotEmpty();

    try 
    {
        while ( isActive() )
        {
            //cout<<"============================================="<<endl;

            // The rangeScanner provides the 'clock' which is the trigger for this loop
            int sensorRet = obsBuffer_.getAndPopNext( rangeData_, TIMEOUT_MS );

            // Before we do anything, check whether we're enabled.
            bool isEnabled;
            enabledPipe_.get( isEnabled );
            if ( !isEnabled )
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
                locBuffer_.get( localiseData_ );
                odomBuffer_.get( odomData_ );

                const double THRESHOLD = 1.0; // seconds
                if ( areTimestampsDodgy( rangeData_, localiseData_, odomData_, THRESHOLD ) )
                {
                    stringstream ss;
                    ss << "Timestamps are more than "<<THRESHOLD<<"sec apart: " << endl
                       << "\t rangeData:    " << orcaice::toString(rangeData_->timeStamp) << endl
                       << "\t localiseData: " << orcaice::toString(localiseData_->timeStamp) << endl
                       << "\t odomData:     " << orcaice::toString(odomData_->timeStamp) << endl
                       << "Maybe something is wrong: Stopping.";
                    context_.tracer()->error( ss.str() );
                    getStopCommand( velocityCmd_ );
                }
                else
                {
                    velocityCmd_->timeStamp = rangeData_->timeStamp;
                    localNavManager_.getCommand( rangeData_,
                                                 localiseData_,
                                                 odomData_,
                                                 velocityCmd_ );
                }
            }
            
            // Send the command to the platform
            try {
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
                              const orca::Localise2dDataPtr   &localiseData, 
                              const orca::Position2dDataPtr   &odomData,
                              double                           threshold )
{
    if ( fabs( orcaice::timeDiffAsDouble( rangeData->timeStamp, localiseData->timeStamp ) ) >= threshold )
        return true;
    if ( fabs( orcaice::timeDiffAsDouble( rangeData->timeStamp, odomData->timeStamp ) ) >= threshold )
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
