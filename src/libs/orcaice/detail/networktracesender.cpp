#include "networktracesender.h"
#include <iostream>
#include <orcaice/orcaice.h>

using namespace std;

namespace orcaice {
namespace detail {

NetworkTraceSender::NetworkTraceSender( const std::string &topicName,
                                        IceUtil::Mutex &mutex,
                                        const orcaice::Context &context )
    : topic_(0),
      publisher_(0),
      topicName_(topicName),
      mutex_(mutex),
      context_(context)
{
}

bool
NetworkTraceSender::connectToIceStorm()
{
    topic_     = 0;
    publisher_ = 0;

    initTracerPrint( orcaice::toString(context_.name())+": Connecting to tracer topic "+ topicName_ );
    try
    {
        topic_ = orcaice::connectToTopicWithString<orca::TracerConsumerPrx>(
            context_, publisher_, topicName_ );
        initTracerPrint( orcaice::toString(context_.name())+": Tracer connected to topic "+topicName_ );
        return true;
    }
    catch ( const orcaice::Exception & e )
    {
        initTracerError( orcaice::toString(context_.name())+": Caught exception while connecting to IceStorm: "+e.what() );
    }
    catch ( Ice::Exception &e )
    {
        std::stringstream s;
        s << orcaice::toString(context_.name()) << ": Caught exception while connecting to IceStorm: " << e;
        initTracerError( s.str() );
    }
    catch ( ... )
    {
        initTracerError( orcaice::toString(context_.name())+": Caught unknown exception while connecting to IceStorm." );
    }
    return false;
}

void
NetworkTraceSender::sendToNetwork( const orca::TracerData &tracerData )
{
    IceUtil::Mutex::Lock lock(mutex_);
    
    // send data
    try
    {
        // This is very tricky, don't touch it if not sure
        // see Connections Issue 5, Avoiding Deadlocks, Part II, File Listing I.
        // If the lock is not released before the remote call, the program locks
        // up on the first remote call. 
        lock.release();
        publisher_->setData( tracerData );
        lock.acquire();
        // end of tricky part.
    }
    catch ( const Ice::CommunicatorDestroyedException & ) // we are not using the exception
    {
        // it's ok, this is what happens on shutdown
        cout<<orcaice::toString(context_.name())<<": tracer: communicator appears to be dead. We must be shutting down."<<endl;
        cout<<orcaice::toString(context_.name())<<": tracer: unsent message: "<<orcaice::toString(tracerData)<<endl;
    }
    catch ( const Ice::Exception &e )
    {
        cout<<orcaice::toString(context_.name())<<": tracer: Caught exception while tracing to topic: "<<e<<endl;
        cout<<orcaice::toString(context_.name())<<": tracer: unsent message: "<<orcaice::toString(tracerData)<<endl;

        // If IceStorm just re-started for some reason, we want to try to re-connect
        connectToIceStorm();
    }
    catch ( ... )
    {
        cout<<orcaice::toString(context_.name())<<": tracer: Caught unknown while tracing to topic."<<endl;
        cout<<orcaice::toString(context_.name())<<": tracer: unsent message: "<<orcaice::toString(tracerData)<<endl;

        // If IceStorm just re-started for some reason, we want to try to re-connect
        connectToIceStorm();
    }
}

void 
NetworkTraceSender::subscribe( const ::orca::TracerConsumerPrx& subscriber )
{
    IceStorm::QoS qos;    
    qos["reliability"] = "twoway";
    topic_->subscribe( qos, subscriber );
}
void 
NetworkTraceSender::unsubscribe( const ::orca::TracerConsumerPrx& subscriber )
{
    topic_->unsubscribe( subscriber );
}

}
}
