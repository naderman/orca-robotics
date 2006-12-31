#include "localise2dI.h"
#include <iostream>
#include <orcaice/orcaice.h>
#include "util.h"

using namespace std;
using namespace orca;

namespace orcaifaceimpl {

Localise2dI::Localise2dI( const std::string &ifaceTag,
                          const orcaice::Context &context )
    : ifaceTag_(ifaceTag),
      context_(context)
{
}

void
Localise2dI::initInterface()
{
    // Find IceStorm Topic to which we'll publish
    topicPrx_ = orcaice::connectToTopicWithTag<orca::Localise2dConsumerPrx>
        ( context_, consumerPrx_, ifaceTag_ );

    // Register with the adapter
    Ice::ObjectPtr obj = this;
    orcaice::createInterfaceWithTag( context_, obj, ifaceTag_ );
}

::orca::Localise2dData 
Localise2dI::getData(const ::Ice::Current& ) const
{
    context_.tracer()->debug( "Localise2dI::getData()", 5 );

    if ( dataProxy_.isEmpty() )
    {
        std::stringstream ss;
        ss << "No data available! (ifaceTag="<<ifaceTag_<<")";
        throw orca::DataNotExistException( ss.str() );
    }

    Localise2dData data;
    dataProxy_.get( data );
    return data;
}

void 
Localise2dI::subscribe(const ::orca::Localise2dConsumerPrx& subscriber, const ::Ice::Current&)
{
    context_.tracer()->debug( "Localise2dI::subscribe()", 5 );
    IceStorm::QoS qos;
    try {
        topicPrx_->subscribe( qos, subscriber );
    }
    catch ( const Ice::Exception & e ) {
        std::stringstream ss;
        ss <<"Localise2dI::subscribe: failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void 
Localise2dI::unsubscribe(const ::orca::Localise2dConsumerPrx& subscriber, const ::Ice::Current&)
{
    context_.tracer()->debug( "Localise2dI::unsubscribe()", 5 );
    topicPrx_->unsubscribe( subscriber );
}

void
Localise2dI::localSetAndSend( const orca::Localise2dData &data )
{
    //cout<<"TRACE(localise2dI.cpp): localSetData: " << orcaice::toString(data) << endl;

    dataProxy_.set( data );
    
    // Try to push to IceStorm.
    tryPushToIceStormWithReconnect<Localise2dConsumerPrx,orca::Localise2dData>
        ( context_,
          consumerPrx_,
          data,
          topicPrx_,
          ifaceTag_ );
}

}
