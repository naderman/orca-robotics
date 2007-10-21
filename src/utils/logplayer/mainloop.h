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
#include <hydroutil/thread.h>

namespace orcalog
{
    class ReplayMaster;
    class Replayer;
    class ReplayFactory;
}

namespace logplayer
{

class MainLoop : public hydroutil::Thread
{
public:

    MainLoop( orcalog::ReplayMaster* master,
              std::vector<orcalog::Replayer*> & replayers,
              std::vector<orcalog::ReplayFactory*> & factories,
              const orcaice::Context & context );
    ~MainLoop(); 
    
    virtual void run();

private:

    orcalog::ReplayMaster* master_;
    std::vector<orcalog::Replayer*> & replayers_;
    std::vector<orcalog::ReplayFactory*> & factories_;
    orcaice::Context context_;
};

} // namespace

#endif
