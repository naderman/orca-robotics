#ifndef MAINTHREAD_H
#define MAINTHREAD_H

#include <orcaice/subsystemthread.h>
#include <orcaice/context.h>
#include <orcaice/ptrbuffer.h>
#include <orcaifaceimpl/imageImpl.h>
#include <orcaifaceimpl/bufferedconsumers.h>

#include <orca/image.h>

namespace orcaimageview
{

class MainThread : public orcaice::SubsystemThread
{
public:
    MainThread( orcaice::PtrBuffer<orca::ImageDataPtr>* imageQueue, const orcaice::Context& context );

    virtual void walk();

private:
    void init();

    //circular buffered consumer for recieving data
    orcaifaceimpl::BufferedImageConsumerImplPtr consumer_;

    // endless queue for display
    orcaice::PtrBuffer<orca::ImageDataPtr>* imageQueue_;

    // context
    orcaice::Context context_;

};

}
#endif

