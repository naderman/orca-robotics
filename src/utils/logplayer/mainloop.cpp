/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#include <iostream>
#include <assert.h>
#include <orcaice/orcaice.h>
#include <orcalog/orcalog.h>

#include "mainloop.h"

using namespace std;
using namespace logplayer;


MainLoop::MainLoop( orcalog::MasterFileReader      *masterFileReader,
              std::vector<orcalog::Replayer*>      &replayers,
              const orcaice::Context               &context )
    :  masterFileReader_(masterFileReader),
       replayers_(replayers),
       context_(context)
{
}

MainLoop::~MainLoop()
{
}

void
MainLoop::readConfig()
{
    // config file parameters
    Ice::PropertiesPtr props = context_.properties();
    string prefix = context_.tag() + ".Config.";

    // BeginTime: Replay data starting from BeginTime (seconds) from the start of the log
    orca::Time tempTime;
    orcaice::setInit( tempTime );
    tempTime = orcaice::getPropertyAsTimeDurationWithDefault( props, prefix+"BeginTime", tempTime );
    beginTime_ = orcaice::toIceTime( tempTime );
    if ( beginTime_<IceUtil::Time() ) {
        beginTime_ = IceUtil::Time();
        context_.tracer()->warning( "Negative BeginTime was reset to 0.0" );
    }

    // EndTime: Replay data up to EndTime (seconds) from the start of the log
    // by convention, negative time means play to the ened
    orcaice::setInit( tempTime );
    tempTime.seconds = -1;
    tempTime = orcaice::getPropertyAsTimeDurationWithDefault( props, prefix+"EndTime", tempTime );
    IceUtil::Time endTime_ = orcaice::toIceTime( tempTime );
    if ( endTime_<IceUtil::Time() ) {
        context_.tracer()->info( "Negative EndTime means the log file will play to the end" );
    }

    // ReplayRate: Adjusts the playback speed
    replayRate_ = orcaice::getPropertyAsDoubleWithDefault( props, prefix+"ReplayRate", 1.0 );
    // special case: wait for human input for every object
    if ( replayRate_ == 0.0 ) {
        waitForUserInput_ = true;
        context_.tracer()->info( "Replay is triggered by user input." );
    }
    else {
        waitForUserInput_ = false;
        clock_.setReplayRate( replayRate_ );
        stringstream ss;
        ss<<"Replay is triggered by the clock (real/replay="<<replayRate_<<").";
        context_.tracer()->info( ss.str() );
    }    

    autoStart_  = orcaice::getPropertyAsIntWithDefault( props, prefix+"AutoStart", 0 );
}

void
MainLoop::initReplayers()
{
    for ( uint i=0; i < replayers_.size(); i++ )
        replayers_[i]->init();
}

void
MainLoop::walk()
{
    // we are in a different thread now, catch all stray exceptions
    try
    {

    readConfig();
    initReplayers();

    if ( !autoStart_ ) {
        cout<<"Type ENTER to start replay:" << endl;
        cout<<"==>"<<endl;
        getchar();
    }

    int seconds = 0;
    int useconds = 0;
    int id = 0;
    int index = 0;

    // Read the first data line
    if ( masterFileReader_->getData( seconds, useconds, id, index ) ) {
        context_.tracer()->error( "Failed to read the 1st line of masterFile" );
        exit(1);
    }

    // fast forward to the desired start time
    // have to do it after we read the first line because begin time
    // is specified since the start of the log
    if ( beginTime_>IceUtil::Time() ) {
        // absolute time of the time from which to start replay
        IceUtil::Time beginLogTime = beginTime_ +
                    IceUtil::Time::seconds(seconds) + IceUtil::Time::microSeconds(useconds);
        cout<<"DEBUG: Begin log absolute time :"<<beginLogTime.toSeconds()<<endl;

        // use special Fast-Forward version of getData()
        if ( masterFileReader_->getDataAfterTime( seconds, useconds, id, index, 
                beginLogTime.toSeconds(), beginLogTime.toMicroSeconds()%1000000 ) ) {
            context_.tracer()->error( "Failed to fast forward to BeginTime "+beginTime_.toDuration() );
            exit(1);
        }

        ostringstream ss;
        ss<<"Fast-forwarded to "<<seconds<<"s, "<<useconds<<"us";
        context_.tracer()->info( ss.str() );
    }

    if ( id > (int)replayers_.size() ) {
        ostringstream ss;
        ss << "Reference to subfile number " << id << ", but only " << replayers_.size() << " exist.";
        context_.tracer()->error( ss.str() );
        exit(1);
    }

    try {
        replayers_[id]->replayData( index );
    }
    catch ( const std::exception  &e) {
        ostringstream ss;
        ss<<"Caught from replayer '"<<replayers_[id]->toString()<<"': "<<e.what();;
        context_.tracer()->warning( ss.str() );
    }

    // Read the start time: this is the offset for the rest of the playback
    clock_.setReplayStartTime( IceUtil::Time::now() );
    clock_.setLogStartTime( IceUtil::Time::seconds(seconds) + IceUtil::Time::microSeconds(useconds) );
    IceUtil::Time nextLogTime;
    IceUtil::Time untilNextLogTime;
    // should this be a configurable parameter? can't think why it would be useful.
    IceUtil::Time replayTimeTolerance = IceUtil::Time::microSeconds(10);

    ///////////////////////////////////////////////////
    //
    // The main loop
    //
    ///////////////////////////////////////////////////
    while( !isStopping() )
    {
        // read a line and act appropriately
        if ( masterFileReader_->getData( seconds, useconds, id, index ) ) 
        {
            // end of file 
            context_.tracer()->debug( "MainLoop: found EOF" );
            break;
        }   
        nextLogTime = IceUtil::Time::seconds(seconds)+IceUtil::Time::microSeconds(useconds);
        
        // check end time (negative end time means play to the end)
        if ( endTime_>IceUtil::Time() && nextLogTime>=endTime_ ) 
        {
            // reached specified end time
            context_.tracer()->debug( "MainLoop: Reached specified end time" );
            break;
        }

        if ( id > (int)replayers_.size() ) {
            ostringstream ss;
            ss << "Reference to subfile number " << id << ", when only " << replayers_.size() << " exist.";
            context_.tracer()->error( ss.str() );
            exit(1);
        }

        // replay is driven by user input
        if ( waitForUserInput_ ) {
            cout<<"Type ENTER to replay the next object:" << endl;
            cout<<"==>"<<endl;
            getchar();
        }
        // replay is driven by the clock
        else {
            // alexm: probably better to do a loop, but for some reason we *always* sleep twice.
            do
            {
                // Work out how long till we should send
                untilNextLogTime = clock_.untilNextLogTime( nextLogTime );
                    
                if ( untilNextLogTime > replayTimeTolerance )
                {
                    IceUtil::ThreadControl::sleep(untilNextLogTime);
                }
                
            } while ( !isStopping() && untilNextLogTime > replayTimeTolerance  );
        }
    
        //
        // Now send it out
        //
        try {
            if ( id >= (int)(replayers_.size()) )
            {
                stringstream ss;
                ss << "Found unknown replayer ID: " << id << ".  Ignoring.";
                context_.tracer()->warning( ss.str() );
            }
            else
            {
                replayers_[id]->replayData( index );
            }
        } 
        catch ( const std::exception  &e) {
            ostringstream ss;
            ss<<"Caught from replayer '"<<replayers_[id]->toString()<<"': "<<e.what();;
            context_.tracer()->warning( ss.str() );
        }

    } // end of main loop
    
    // we can only get here if it's the end of the logfile
    context_.tracer()->info( "Reached the end of log file." );
    
    
    // we are done here. So to let the application know that we are finished,
    // we destroy the communicator, triggering clean-up and shutdown.
    context_.communicator()->destroy();

    } // try
    // we'll get this exception on Ctrl-C
    catch ( const Ice::CommunicatorDestroyedException  &e )
    {
        // it's ok, we must be quitting.
    }
    catch ( const Ice::Exception  &e )
    {
        stringstream ss;
        ss << "MainLoop: unexpected Ice exception: " << e;
        context_.tracer()->error( ss.str() );
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const std::exception  &e )
    {
        stringstream ss;
        ss << "MainLoop: unexpected std exception: " << e.what();
        context_.tracer()->error( ss.str() );
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( ... )
    {
        context_.tracer()->error( "MainLoop: unexpected unknown exception.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }

    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
    
    context_.tracer()->debug( "Exiting MainLoop thread", 2 );
}
