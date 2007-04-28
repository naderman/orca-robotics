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
                          orcaice::Buffer<orca::Localise2dData> &locBuffer )
    : topic_(topic),
      locBuffer_(locBuffer)
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
    topic_->subscribeAndGetPublisher( IceStorm::QoS(), subscriber->ice_twoway() );
}

void 
Localise2dI::unsubscribe(const ::orca::Localise2dConsumerPrx& subscriber, const ::Ice::Current&)
{
    cout<<"unsubscribe()"<<endl;
    topic_->unsubscribe( subscriber );
}
