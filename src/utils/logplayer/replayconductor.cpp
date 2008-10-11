#include "replayconductor.h"
#include <iostream>
#include <orcalog/utils.h>
#include <hydroutil/hydroutil.h>
#include <limits>

using namespace std;

namespace logplayer {

ReplayConductor::ReplayConductor( orcalog::MasterFileReader       &masterFileReader,
                                  std::vector<orcalog::Replayer*> &replayers,
                                  const IceUtil::Time             &beginTime,
                                  double                           replayRate,
                                  const orcaice::Context          &context )
    : SubsystemThread( context.tracer(), context.status() ),
      isPlaying_(false),
      isPlayingOrAboutToStart_(false),
      masterFileReader_(masterFileReader),
      replayers_(replayers),
      context_(context)
{
    clock_.setReplayRate(replayRate);

    bool cursorValid = masterFileReader_.getCursorTime( firstItemSec_, firstItemUsec_ );
    assert( cursorValid );

    // Fast-forward if non-zero beginTime
    if ( beginTime > orcalog::iceUtilTime(0,0) )
    {
        fastForward( beginTime );
    }
}

void
ReplayConductor::addEvent( const Event &e )
{
    eventQueue_.push( e );
    stringstream ss;
    ss << "ReplayConductor: Received event: " << toString(e);
    context_.tracer().info( ss.str() );    
}

void
ReplayConductor::setReplayRate( double rate )
{
    IceUtil::Mutex::Lock lock(mutex_);

    // If we're playing, pause before adjusting rate.
    if ( isPlayingOrAboutToStart_ )
        eventQueue_.push( Event( Pause ) );

    addEvent( Event( SetReplayRate, rate ) );

    // Then un-pause if necessary
    if ( isPlayingOrAboutToStart_ )
        eventQueue_.push( Event( Start ) );
}

void
ReplayConductor::startPlaying()
{
    IceUtil::Mutex::Lock lock(mutex_);
    addEvent( Event( Start ) );
    isPlayingOrAboutToStart_ = true;
}
    
void
ReplayConductor::pausePlaying()
{
    IceUtil::Mutex::Lock lock(mutex_);
    addEvent( Event( Pause ) );
}

bool
ReplayConductor::isPlayingOrAboutToStart()
{
    IceUtil::Mutex::Lock lock(mutex_);
    return isPlayingOrAboutToStart_;
}

bool
ReplayConductor::isPlaying()
{
    IceUtil::Mutex::Lock lock(mutex_);
    return isPlaying_;
}

void
ReplayConductor::stepForward()
{
    IceUtil::Mutex::Lock lock(mutex_);
    addEvent( Event( StepForward ) );
}

void
ReplayConductor::stepBackward()
{
    IceUtil::Mutex::Lock lock(mutex_);
    addEvent( Event( StepBackward ) );
}

void
ReplayConductor::fastForward( const IceUtil::Time &time )
{
    IceUtil::Mutex::Lock lock(mutex_);
    addEvent( Event( FastForward, time ) );
}

void
ReplayConductor::fastForwardToEnd()
{
    IceUtil::Mutex::Lock lock(mutex_);
    addEvent( Event( FastForwardToEnd ) );
}

void
ReplayConductor::rewind( const IceUtil::Time &time )
{
    IceUtil::Mutex::Lock lock(mutex_);
    addEvent( Event( Rewind, time ) );
}

void
ReplayConductor::rewindToStartAndStop()
{
    IceUtil::Mutex::Lock lock(mutex_);
    addEvent( Event( RewindToStartAndStop ) );
}

bool
ReplayConductor::getCursorTime( int &sec, int &usec )
{
    IceUtil::Mutex::Lock lock(mutex_);
    return masterFileReader_.getCursorTime( sec, usec );
}

void
ReplayConductor::handleEvents()
{
    IceUtil::Mutex::Lock lock(mutex_);

    // Pop all the events off the queue and deal with them.
    while ( !eventQueue_.empty() )
    {
        Event e = eventQueue_.front();
        eventQueue_.pop();

        switch ( e.type() )
        {
        case SetReplayRate:
            handleSetReplayRate(e.replayRate());
            break;
        case Start:
            handleStart();
            break;
        case Pause:
            handlePause();
            break;
        case StepForward:
            handleStepForward();
            break;
        case StepBackward:
            handleStepBackward();
            break;
        case FastForward:
            handleFastForward(e.time());
            break;
        case FastForwardToEnd:
            handleFastForwardToEnd();
            break;
        case Rewind:
            handleRewind(e.time());
            break;
        case RewindToStartAndStop:
            handleRewindToStartAndStop();
            break;
        default:
            cout << "ERROR(replayconductor.cpp): Don't know how to handle event: " << toString(e) << endl;
        }
    }
}

void
ReplayConductor::handleSetReplayRate( double rate )
{
    bool wasPlaying = isPlaying_;
    if ( wasPlaying )
        handlePause();

    clock_.setReplayRate( rate );
    
    if ( wasPlaying )
        handleStart();
}

void
ReplayConductor::handleStart()
{
    if ( isPlaying_ )
    {
        // We're playing already.  Nothing to do.
        return;
    }
    int sec, usec;
    bool cursorValid = masterFileReader_.getCursorTime( sec, usec );
    if ( !cursorValid )
    {
        context_.tracer().debug( "ReplayConductor: At end of log.  Can't start." );
        isPlayingOrAboutToStart_ = false;
        return;
    }
    clock_.setContinuousReplayStartTime( orcalog::iceUtilTime( sec, usec ) );

    // cout<<"TRACE(replayconductor.cpp): will start at " << toString(orcalog::iceUtilTime(sec,usec)) << endl;
        
    // This flag will cause the main loop to start rolling.
    isPlaying_ = true;
    isPlayingOrAboutToStart_ = true;
}

void
ReplayConductor::handlePause()
{
    isPlaying_ = false;
    isPlayingOrAboutToStart_ = false;
    return;
}

void
ReplayConductor::handleRewindToStartAndStop()
{
    masterFileReader_.rewindToStart();
    eventQueue_.push( Event( Pause ) );
}

void
ReplayConductor::handleFastForwardToEnd()
{
    int sec, usec, id, index;
    masterFileReader_.getDataAtOrAfterTime( sec, usec, id, index,
                                            std::numeric_limits<int>::max(),
                                            std::numeric_limits<int>::max() );
}

void
ReplayConductor::handleRewind( const IceUtil::Time &deltaT )
{
    bool wasPlaying = isPlaying_;
    if ( wasPlaying )
        handlePause();

    int sec, usec;
    masterFileReader_.getCursorTime( sec, usec );

    IceUtil::Time tNew = orcalog::iceUtilTime( sec, usec );
    tNew -= deltaT;
    masterFileReader_.placeCursorAtOrAfterTime( tNew.toSeconds(),
                                                (int)(tNew.toMicroSeconds()-tNew.toSeconds()*1e6) );

    if ( wasPlaying )
        handleStart();
}

void
ReplayConductor::handleFastForward( const IceUtil::Time &deltaT )
{
    bool wasPlaying = isPlaying_;
    if ( wasPlaying )
        handlePause();

    int sec, usec;
    masterFileReader_.getCursorTime( sec, usec );

    IceUtil::Time tNew = orcalog::iceUtilTime( sec, usec );
    tNew += deltaT;
    masterFileReader_.placeCursorAtOrAfterTime( tNew.toSeconds(),
                                                (int)(tNew.toMicroSeconds()-tNew.toSeconds()*1e6) );
    

    //masterFileReader_.getCursorTime( sec, usec );

    if ( wasPlaying )
        handleStart();
}

void
ReplayConductor::handleStepForward()
{
    if ( isPlaying_ )
    {
        context_.tracer().warning( "ReplayConductor: Can't step forward while playing." );
        return;
    }

    int seconds, useconds, id, index;
    int ret = masterFileReader_.getData( seconds, useconds, id, index );    
    if ( ret != 0 )
    {
        context_.tracer().info( "ReplayConductor::handleStepForward: end of file reached." );
    }
    else
    {
        replayData( id, index );
    }
}

void
ReplayConductor::handleStepBackward()
{
    if ( isPlaying_ )
    {
        context_.tracer().warning( "ReplayConductor: Can't step backward while playing." );
        return;
    }

    // Step back _twice_, then forward once.
    masterFileReader_.stepBackward();
    masterFileReader_.stepBackward();
    
    int seconds, useconds, id, index;
    int ret = masterFileReader_.getData( seconds, useconds, id, index );    
    if ( ret != 0 )
    {
        context_.tracer().error( "ReplayConductor::handleStepBackward: getData returned non-zero!" );
    }
    else
    {
        replayData( id, index );
    }
}

void
ReplayConductor::replayData( int id, int index )
{
    if ( id >= (int)(replayers_.size()) )
    {
        stringstream ss;
        ss << "ReplayConductor: Found unknown replayer ID: " << id << ".  Ignoring.";
        context_.tracer().warning( ss.str() );
        
        return;
    }

    try {
        replayers_[id]->replayData( index );
    }
    catch ( const Ice::CommunicatorDestroyedException  &e ) 
    {
        // This is OK, it means we're shutting down.
        throw;
    }
    catch ( const std::exception  &e ) 
    {
        stringstream ss;
        ss<<"ReplayConductor: Caught std::exception from replayer '"<<replayers_[id]->toString()<<"': "<<e.what();
        context_.tracer().error( ss.str() );
    }
    catch ( ... )
    {
        stringstream ss;
        ss<<"ReplayConductor: Caught unknown exception from replayer '"<<replayers_[id]->toString();
        context_.tracer().error( ss.str() );
    }
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
        if ( !isPlaying() )
        {
            // Slow the loop down a bit...            
            usleep( (int)(0.1 * 1e6) );
            continue;
        }

        //
        // If we get to here we're playing.
        //

        // read a line and act appropriately
        if ( masterFileReader_.getData( seconds, useconds, id, index ) ) 
        {
            // end of file 
            context_.tracer().info( "ReplayConductor: End of File." );
            pausePlaying();
            continue;
        }
        nextLogTime = orcalog::iceUtilTime( seconds, useconds );
        
        if ( id > (int)replayers_.size() ) {
            stringstream ss;
            ss << "ReplayConductor: Reference to subfile number " << id << ", when only " << replayers_.size() << " exist.";
            context_.tracer().error( ss.str() );
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
                    context_.tracer().warning( "ReplayConductor: long time between replays, could screw with interactivity." );
                }
                IceUtil::ThreadControl::sleep(realTimeTillNextItem);
            }
                
        } while ( !isStopping() && realTimeTillNextItem > replayTimeTolerance  );

        //
        // Now send it out
        //
        // cout<<"TRACE(replayconductor.cpp): replayData("<<index<<", t="<<toString(nextLogTime)<<")" << endl;
        replayData( id, index );

    } // end of main loop
}

std::string 
ReplayConductor::toString( EventType e )
{
    stringstream ss;
    switch (e)
    {
    case SetReplayRate:
        ss << "SetReplayRate";
        break;
    case Start:
        ss << "Start";
        break;
    case Pause:
        ss << "Pause";
        break;
    case StepForward:
        ss << "StepForward";
        break;
    case StepBackward:
        ss << "StepBackward";
        break;
    case FastForward:
        ss << "FastForward";
        break;
    case FastForwardToEnd:
        ss << "FastForwardToEnd";
        break;
    case Rewind:
        ss << "Rewind";
        break;
    case RewindToStartAndStop:
        ss << "RewindToStartAndStop";
        break;
    default:
        ss << "Unknown EventType: " << e;
        assert( false && "Unknown EventType" );
    }
    return ss.str();
}

std::string 
ReplayConductor::toString( const Event &e )
{
    stringstream ss;
    ss << toString(e.type());
    switch ( e.type() )
    {
    case SetReplayRate:
        ss << "("<<e.replayRate()<<")";
        break;
    case Start:
        break;
    case Pause:
        break;
    case StepForward:
        break;
    case StepBackward:
        break;
    case FastForward:
        ss << "("<<toString(e.time())<<")";
        break;
    case FastForwardToEnd:
        break;
    case Rewind:
        ss << "("<<toString(e.time())<<")";
        break;
    case RewindToStartAndStop:
        break;
    default:
        ss << "Unknown EventType: " << e.type();
        assert( false && "Unknown EventType" );
    }
    return ss.str();
}

std::string 
ReplayConductor::toString( const IceUtil::Time &t )
{
    stringstream ss;
    ss << t.toSeconds() << ":" << t.toMicroSeconds()-t.toSeconds()*1e6;
    return ss.str();
}

}

