/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_LOGPLAYER_MAINLOOP_H
#define ORCA2_LOGPLAYER_MAINLOOP_H

#include <vector>

#include <orcaice/context.h>
#include <hydroutil/safethread.h>
#include "replayclock.h"

namespace orcalog
{
    class MasterFileReader;
    class Replayer;
    class ReplayFactory;
}

namespace logplayer
{

class MainLoop : public hydroutil::SafeThread
{
public:

    MainLoop( orcalog::MasterFileReader       *masterFileReader,
              std::vector<orcalog::Replayer*> &replayers,
              const orcaice::Context          &context );
    ~MainLoop(); 

    // from SafeThread
    virtual void walk();

private:

    void readConfig();
    void initReplayers();

    orcalog::MasterFileReader* masterFileReader_;
    std::vector<orcalog::Replayer*> & replayers_;
    orcaice::Context context_;

    // The clock by which replay occurs (might not run in real-time).
    ReplayClock clock_;

    // Params read from config file

    // Replay data starting from BeginTime (seconds) from the start of the log
    IceUtil::Time beginTime_;

    // EndTime: Replay data up to EndTime (seconds) from the start of the log
    // by convention, negative time means play to the ened
    IceUtil::Time endTime_;

    // ReplayRate: Adjusts the playback speed (1.0 is real-time).
    double replayRate_;

    // Start automatically? (or wait for user key-press)
    bool autoStart_;

    // replay one data item per keypress?
    bool waitForUserInput_;
};

} // namespace

#endif
