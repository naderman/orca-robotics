/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <iostream>

#include "localise2dI.h"

using namespace std;
using namespace orca;
using namespace faithlocaliser;

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
    
    locBuffer_.get( data );
    return data;
}

void 
Localise2dI::subscribe(const ::orca::Localise2dConsumerPrx& subscriber, const ::Ice::Current&)
{
//     cout<<"subscribe()"<<endl;
    IceStorm::QoS qos;
    localiseTopic_->subscribe( qos, subscriber );
}

void 
Localise2dI::unsubscribe(const ::orca::Localise2dConsumerPrx& subscriber, const ::Ice::Current&)
{
//     cout<<"unsubscribe()"<<endl;
    localiseTopic_->unsubscribe( subscriber );
}
