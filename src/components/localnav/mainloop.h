#ifndef MAINLOOP_H
#define MAINLOOP_H

#include <orca/platform2d.h>
#include <orca/localise2d.h>
#include <orca/rangescanner.h>
#include <orcaice/context.h>
#include <orcaice/ptrbuffer.h>
#include <orcaice/thread.h>
#include <orcaice/heartbeater.h>

#include <localnavmanager.h>

namespace localnav {

class PathMaintainer;
class PathFollower2dI;

//
// @brief the main executing loop of this component.
//
// Note: this thing self-destructs when run() returns.
//
//
// @author Alex Brooks
//
class MainLoop : public orcaice::Thread
{

public: 

    MainLoop( LocalNavManager                               &localNavManager,
              orcaice::PtrBuffer<orca::RangeScannerDataPtr> &obsBuffer,
              orcaice::PtrBuffer<orca::Localise2dDataPtr>   &locBuffer,
              orcaice::PtrBuffer<orca::Position2dDataPtr>   &odomBuffer,
              orca::Platform2dPrx                           &platform2dPrx,
              PathMaintainer                                &pathMaintainer,
              orca::PathFollower2dConsumerPrx               &pathPublisher,
              orcaice::Context                               context );
    ~MainLoop();

    virtual void run();

private: 

    // Make sure all our sources of info are OK, and that there's something
    // in all our buffers
    void ensureBuffersNotEmpty();

    // Set the command to 'stop'
    void getStopCommand( orca::Velocity2dCommandPtr cmd );

    // See if we need to follow a new path, plus
    // see if we should update the world on our progress.
    void checkWithOutsideWorld( PathMaintainer &pathMaintainer );

    // Returns true if the timestamps differ by more than a threshold.
    bool areTimestampsDodgy( const orca::RangeScannerDataPtr &rangeData,
                             const orca::Localise2dDataPtr   &localiseData,
                             const orca::Position2dDataPtr   &odomData );

    void maybeSendHeartbeat();

    // The class that does the navigating
    LocalNavManager &localNavManager_;

    // Incoming observations and pose info
    orcaice::PtrBuffer<orca::RangeScannerDataPtr> &obsBuffer_;
    orcaice::PtrBuffer<orca::Localise2dDataPtr>   &locBuffer_;
    orcaice::PtrBuffer<orca::Position2dDataPtr>   &odomBuffer_;

    // data types
    orca::Localise2dDataPtr      localiseData_;
    orca::Position2dDataPtr      odomData_;
    orca::RangeScannerDataPtr    rangeData_;
    orca::Velocity2dCommandPtr   velocityCmd_;

    // Outgoing commands
    orca::Platform2dPrx &platform2dPrx_;

    // Keeps track of the path we're following
    PathMaintainer  &pathMaintainer_;

    // The object to publish path updates to
    orca::PathFollower2dConsumerPrx              &pathPublisher_;

    orcaice::Heartbeater heartbeater_;

    orcaice::Context context_;
};

}

#endif
