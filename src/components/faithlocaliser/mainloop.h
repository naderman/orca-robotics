#ifndef ORCA2_FAITH_LOCALISER_MAIN_LOOP_H
#define ORCA2_FAITH_LOCALISER_MAIN_LOOP_H

#include <orcaice/thread.h>
#include <orcaice/ptrbuffer.h>
#include <orcaice/context.h>
#include <orca/localise2d.h>

namespace faithlocaliser
{

// Note: this thing self-destructs when run() returns.
class MainLoop : public orcaice::Thread
{

public:

    MainLoop( const orca::Localise2dConsumerPrx                localise2dConsumer,
              orcaice::PtrBuffer<orca::Position2dDataPtr>     &posBuffer,
              orcaice::PtrBuffer<orca::Localise2dDataPtr>     &locBuffer,
              orcaice::Context                                 context );
    ~MainLoop();

    virtual void run();

private:

    // IceStorm consumer
    const orca::Localise2dConsumerPrx  localise2dConsumer_;

    // incoming
    orcaice::PtrBuffer<orca::Position2dDataPtr> &posBuffer_;
    
    // outgoing
    orcaice::PtrBuffer<orca::Localise2dDataPtr> &locBuffer_;

    orcaice::Context context_;
};

} // namespace

#endif
