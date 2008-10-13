#include "mainthread.h"

#include <orcaice/orcaice.h>

using namespace orcaimageview;

MainThread::MainThread( orcaice::PtrBuffer<orca::ImageDataPtr>* imageQueue, const orcaice::Context& context )
: SubsystemThread( context.tracer(), context.status(), "MainThread" )
, imageQueue_(imageQueue)
, context_(context)
{
    subStatus().setMaxHeartbeatInterval( 20.0 );
}

void
MainThread::walk()
{
    // multi-try function
    orcaice::activate( context_, this, subsysName() );

    // data structures
    orca::ImageDataPtr imageData;

    // READ CONFIG
    std::string prefix = context_.tag() + ".Config.";
    
    int sleepIntervalMs = orcaice::getPropertyAsIntWithDefault( context_.properties(), prefix+"SleepIntervalMs", 100 );
    subStatus().ok( "Initialized" );

    // SUBSCRIBE
    orcaifaceimpl::BufferedImageConsumerImplPtr imageInterface = new orcaifaceimpl::BufferedImageConsumerImpl( 10, gbxiceutilacfr::BufferTypeCircular, context_ );

    // try multiple times
    imageInterface->subscribeWithTag( "Image", this, subsysName() );

    //check every so often to see if we should quit
    const int timeoutMs = 500;

    // LOOP
    subStatus().setMaxHeartbeatInterval( sleepIntervalMs * 3.0/1000.0 );
    while( !isStopping() )
    {
        context_.tracer().debug("Fetching Image");

        int ret = imageInterface->buffer().getAndPopNext( imageData, timeoutMs );
            
        if(!ret)
        {
            context_.tracer().debug("Successfuly fetched Image");
            context_.tracer().debug("Adding Image to Queue");
            imageQueue_->push(imageData);
        }
    }
}
