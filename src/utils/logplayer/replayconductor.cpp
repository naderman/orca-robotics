#include "replayconductor.h"
#include <iostream>
#include <orcalog/utils.h>
#include <hydroutil/hydroutil.h>

using namespace std;

namespace logplayer {

ReplayConductor::ReplayConductor( orcalog::MasterFileReader       &masterFileReader,
                                  std::vector<orcalog::Replayer*> &replayers,
                                  const IceUtil::Time             &beginTime,
                                  double                           replayRate,
                                  const orcaice::Context          &context )
    : isPlaying_(false),
      isPlayingOrAboutToStart_(false),
      masterFileReader_(masterFileReader),
      replayers_(replayers),
      context_(context)
{
    clock_.setReplayRate(replayRate);

    // Fast-forward if non-zero beginTime
    if ( beginTime > orcalog::iceUtilTime(0,0) )
    {
        fastForward( beginTime );
    }
}

ReplayConductor::~ReplayConductor()
{
}

void
ReplayConductor::setReplayRate( double rate )
{
    IceUtil::Mutex::Lock lock(mutex_);
    stringstream ss;
    ss << "ReplayConductor: Received new replay rate: " << rate;
    context_.tracer()->info( ss.str() );
    eventQueue_.push( Event( SetReplayRate, rate ) );
}

void
ReplayConductor::startPlaying()
{
    IceUtil::Mutex::Lock lock(mutex_);
    stringstream ss;
    ss << "ReplayConductor: Received startPlaying instruction.";
    context_.tracer()->info( ss.str() );
    eventQueue_.push( Event( Start ) );
    isPlayingOrAboutToStart_ = true;
}
    
void
ReplayConductor::pausePlaying()
{
    IceUtil::Mutex::Lock lock(mutex_);
    stringstream ss;
    ss << "ReplayConductor: Received pausePlaying instruction.";
    context_.tracer()->info( ss.str() );
    eventQueue_.push( Event( Pause ) );
}

bool
ReplayConductor::isPlayingOrAboutToStart()
{
    IceUtil::Mutex::Lock lock(mutex_);
    return isPlayingOrAboutToStart_;
}

void
ReplayConductor::stepForward()
{
}

void
ReplayConductor::stepBackward()
{
}

void
ReplayConductor::fastForward( const IceUtil::Time &time )
{
}

void
ReplayConductor::fastForwardToEnd()
{
}

void
ReplayConductor::rewind( const IceUtil::Time &time )
{
}

void
ReplayConductor::rewindToStartAndStop()
{
}

void
ReplayConductor::handleEvents()
{
    IceUtil::Mutex::Lock lock(mutex_);

    // Pop all the events off the queue and deal with them.
    while ( !eventQueue_.empty() )
    {
        handleEvent( eventQueue_.front() );
        eventQueue_.pop();
    }
}

void
ReplayConductor::handleEvent( const Event &event )
{
    switch ( event.type() )
    {
    case SetReplayRate:
    {
        clock_.setReplayRate( event.replayRate() );
        break;
    }
    case Start:
    {
        if ( isPlaying_ )
        {
            // We're playing already.  Nothing to do.
            break;
        }
        IceUtil::Time timeNextItem;
        int ret = getTimeOfNextItem( timeNextItem );
        if ( ret != 0 )
        {
            context_.tracer()->debug( "ReplayConductor: At end of log.  Can't start." );
            break;
        }
        clock_.setContinuousReplayStartTime( timeNextItem );

        // This flag will cause the main loop to start rolling.
        isPlaying_ = true;
        isPlayingOrAboutToStart_ = true;
        break;
    }
    case Pause:
    {
        isPlaying_ = false;
        isPlayingOrAboutToStart_ = false;
        break;
    }
    default:
    {
        throw hydroutil::Exception( ERROR_INFO, "Don't know how to handle this event type!" );
        break;
    }
    }
}

int
ReplayConductor::getTimeOfNextItem( IceUtil::Time &timeNextItem )
{
    int seconds, useconds, id, index;
    if ( masterFileReader_.getData( seconds, useconds, id, index ) != 0 ) 
    {
        return -1;
    }
    else
    {
        timeNextItem = orcalog::iceUtilTime( seconds, useconds );
        // now rewind by one item
        masterFileReader_.placeCursorBeforeTime( seconds, useconds );
    }
    return 0;
}

void
ReplayConductor::walk()
{
    int id, index;
    int seconds, useconds;
    IceUtil::Time nextLogTime;
    IceUtil::Time realTimeTillNextItem;

    // should this be a configurable parameter? can't think why it would be useful.
    IceUtil::Time replayTimeTolerance = IceUtil::Time::microSeconds(10);

    while( !isStopping() )
    {
        handleEvents();
        // Check if we're playing
        {
            IceUtil::Mutex::Lock lock(mutex_);
            if ( !isPlaying_ )
                continue;
        }

        //
        // If we get to here we're playing.
        //

        // read a line and act appropriately
        if ( masterFileReader_.getData( seconds, useconds, id, index ) ) 
        {
            // end of file 
            cout<<"TRACE(replayconductor.cpp): End of File." << endl;
            pausePlaying();
            continue;
        }
        nextLogTime = orcalog::iceUtilTime( seconds, useconds );
        
        if ( id > (int)replayers_.size() ) {
            stringstream ss;
            ss << "ReplayConductor: Reference to subfile number " << id << ", when only " << replayers_.size() << " exist.";
            context_.tracer()->error( ss.str() );
            exit(1);
        }

        do
        {
            // Work out how long till we should send
            realTimeTillNextItem = clock_.realTimeTillNextItem( nextLogTime );

            if ( realTimeTillNextItem > replayTimeTolerance )
            {
                if ( realTimeTillNextItem > orcalog::iceUtilTime(1,0) )
                {
                    context_.tracer()->warning( "ReplayConductor: long time between replays, could screw with interactivity." );
                }
                IceUtil::ThreadControl::sleep(realTimeTillNextItem);
            }
                
        } while ( !isStopping() && realTimeTillNextItem > replayTimeTolerance  );

        //
        // Now send it out
        //
        try {
            if ( id >= (int)(replayers_.size()) )
            {
                stringstream ss;
                ss << "ReplayConductor: Found unknown replayer ID: " << id << ".  Ignoring.";
                context_.tracer()->warning( ss.str() );
            }
            else
            {
                replayers_[id]->replayData( index );
            }
        } 
        catch ( const std::exception  &e) 
        {
            stringstream ss;
            ss<<"ReplayConductor: Caught exception from replayer '"<<replayers_[id]->toString()<<"': "<<e.what();
            context_.tracer()->error( ss.str() );
        }

    } // end of main loop
}


}

