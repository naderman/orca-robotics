#ifndef LOGPLAYER_REPLAYCONDUCTOR_H
#define LOGPLAYER_REPLAYCONDUCTOR_H

#include <hydroutil/safethread.h>
#include <orcaice/context.h>
#include <orcalog/orcalog.h>
#include <IceUtil/IceUtil.h>
#include "replayclock.h"
#include <queue>

namespace logplayer {

//!
//! @brief Orchestrates replaying of the log.
//!        This thing is a thread, so it can play continuously.
//!        It's thread-safe so that replay-rate etc can be adjusted during operation.
//!
//! @author Alex Brooks
//!
class ReplayConductor : public hydroutil::SafeThread
{

public: 

    // beginTime is relative to the start of the log (not absolute)
    ReplayConductor( orcalog::MasterFileReader       &masterFileReader,
                     std::vector<orcalog::Replayer*> &replayers,
                     const IceUtil::Time             &beginTime,
                     double                           replayRate,
                     const orcaice::Context          &context );
    ~ReplayConductor();

    // 'rate' is a multiple of real-time (ie >1.0 means faster-than-real-time)
    void setReplayRate( double rate );

    // Commence continuous playing (at the specified rate)
    // Equivalent to 'unpause'.
    void startPlaying();
    
    // Stop playing and remember current position
    void pausePlaying();

    // false if paused, not yet started, or at end of log
    // the 'aboutToStart' case is if we've received a Play event
    // but haven't processed it yet.
    bool isPlayingOrAboutToStart();

    // Step forward by one.  Only valid when not playing.
    void stepForward();

    // Step backward by one.  Only valid when not playing.
    void stepBackward();

    // Go forwards by the prescribed (relative) amount
    void fastForward( const IceUtil::Time &time );

    // go to the end of the log
    void fastForwardToEnd();

    // Go backwards by the prescribed (relative) amount
    void rewind( const IceUtil::Time &time );

    // Rewind to the start of the log and don't start playing again yet.
    void rewindToStartAndStop();

    // This is the main thread function.
    void walk();

private:

    ////////////////////////////////////////

    enum EventType {
        SetReplayRate,
        Start,
        Pause,
//         StepForward,
//         StepBackward,
//         FastForward,
//         FastForwardToEnd,
//         Rewind,
//         RewindToStartAndStop
    };
    class Event {
    public:
        Event( EventType eventType )
            : type_(eventType) {}
        Event( EventType eventType, double replayRate )
            : type_(eventType),
              replayRate_(replayRate)
            {}
//        Event( EventType eventType, int sec, int usec );
        EventType type() const { return type_; }
        double replayRate() const
            {
                assert( type_ == SetReplayRate );
                return replayRate_; 
            }
    private:
        const EventType type_;
        double          replayRate_;
        IceUtil::Time   time_;
    };

    ////////////////////////////////////////

    void handleEvents();
    void handleEvent( const Event &event );
    int getTimeOfNextItem( IceUtil::Time &timeNextItem );

    // Handles high-level commands
    std::queue<Event> eventQueue_;

    IceUtil::Mutex   mutex_;

    // This is if we actually are playing
    bool isPlaying_;

    // This is for outsiders: if we're either playing or have received
    // a Play event that's not processed yet.
    bool isPlayingOrAboutToStart_;

    // The clock by which replay occurs (might not run in real-time).
    ReplayClock clock_;

    orcalog::MasterFileReader       &masterFileReader_;
    std::vector<orcalog::Replayer*> &replayers_;
    orcaice::Context                 context_;
};

}

#endif
