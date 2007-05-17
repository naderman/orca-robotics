/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include "localise2dI.h"

using namespace std;
using namespace orca;
using namespace simlocaliser;

Localise2dI::Localise2dI( const IceStorm::TopicPrx              &topic,
                          orcaice::Buffer<orca::Localise2dData> &locBuffer,
                          const orcaice::Context &context )
    : topicPrx_(topic),
      locBuffer_(locBuffer),
      context_(context)
{
}

::orca::Localise2dData
Localise2dI::getData(const ::Ice::Current& ) const
{
    Localise2dData data;

    // we don't need to pop the data here because we don't block on it.
    // we always want to have the latest copy in there
    try
    {
        locBuffer_.get( data );
    }
    catch ( const orcaice::Exception & e )
    {
        throw orca::DataNotExistException( "localisation buffer is not populated yet" );
    }
    
    return data;
}

// this implementation does not interpolate
::orca::Localise2dData
Localise2dI::getDataAtTime(const orca::Time&, const ::Ice::Current& ) const
{
    Localise2dData data;

    // we don't need to pop the data here because we don't block on it.
    // we always want to have the latest copy in there
    try
    {
        locBuffer_.get( data );
    }
    catch ( const orcaice::Exception & e )
    {
        throw orca::DataNotExistException( "localisation buffer is not populated yet" );
    }
    
    return data;
}

void 
Localise2dI::subscribe(const ::orca::Localise2dConsumerPrx& subscriber, const ::Ice::Current&)
{
    cout<<"subscribe()"<<endl;
    try {
        topicPrx_->subscribeAndGetPublisher( IceStorm::QoS(), subscriber->ice_twoway() );
    }
    catch ( const IceStorm::AlreadySubscribed & e ) {
        std::stringstream ss;
        ss <<"Request for subscribe but this proxy has already been subscribed, so I do nothing: "<< e;
        context_.tracer()->debug( ss.str(), 2 );    
    }
    catch ( const Ice::Exception & e ) {
        std::stringstream ss;
        ss <<"subscribe: failed to subscribe: "<< e << endl;
        context_.tracer()->warning( ss.str() );
        throw orca::SubscriptionFailedException( ss.str() );
    }
}

void 
Localise2dI::unsubscribe(const ::orca::Localise2dConsumerPrx& subscriber, const ::Ice::Current&)
{
    cout<<"unsubscribe()"<<endl;
    topicPrx_->unsubscribe( subscriber );
}
