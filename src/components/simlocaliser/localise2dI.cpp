/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>

#include "localise2dI.h"

using namespace std;
using namespace orca;
using namespace simlocaliser;

Localise2dI::Localise2dI( const IceStorm::TopicPrx              &localiseTopic,
                          orcaice::PtrBuffer<Localise2dDataPtr> &locBuffer )
    : localiseTopic_(localiseTopic),
      locBuffer_(locBuffer)
{
}

::orca::Localise2dDataPtr 
Localise2dI::getData(const ::Ice::Current& ) const
{
    // create a null pointer. data will be cloned into it.
    Localise2dDataPtr data;

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

::orca::Localise2dDataPtr
Localise2dI::getDataAtTime(const orca::Time&, const ::Ice::Current& ) const
{
    return NULL;
}

void 
Localise2dI::subscribe(const ::orca::Localise2dConsumerPrx& subscriber, const ::Ice::Current&)
{
    cout<<"subscribe()"<<endl;
    IceStorm::QoS qos;
    localiseTopic_->subscribe( qos, subscriber );
}

void 
Localise2dI::unsubscribe(const ::orca::Localise2dConsumerPrx& subscriber, const ::Ice::Current&)
{
    cout<<"unsubscribe()"<<endl;
    localiseTopic_->unsubscribe( subscriber );
}
