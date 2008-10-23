/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcacm/orcacm.h>
#include <orcaprobe/orcaprobe.h>
#include <orcaifacestring/wifi.h>

#include "wifiprobe.h"

using namespace std;
using namespace orcaprobefactory;

WifiProbe::WifiProbe( const orca::FQInterfaceName& name,
                      orcaprobe::AbstractDisplay& display,                      
                      const orcaice::Context& context ) : 
    InterfaceProbe(name,display,context)
{
    id_ = "::orca::Wifi";

    addOperation( "getData",        "nonmutating WifiData getData()" );
    addOperation( "subscribe",      "void subscribe( WifiConsumer *subscriber )" );
    addOperation( "unsubscribe",    "idempotent void unsubscribe( WifiConsumer *subscriber )" );

    consumer_ = new orcaifaceimpl::PrintingWifiConsumerImpl( context,1000,1 );
}
    
WifiProbe::~WifiProbe()
{
}
    
int 
WifiProbe::loadOperationEvent( const int index, orcacm::OperationData& data )
{    
    switch ( index )
    {
    case orcaprobe::UserIndex :
        return loadGetData( data );
    case orcaprobe::UserIndex+1 :
        return loadSubscribe( data );
    case orcaprobe::UserIndex+2 :
        return loadUnsubscribe( data );
    }
    return 1;
}

int 
WifiProbe::loadGetData( orcacm::OperationData& data )
{
    orca::WifiPrx derivedPrx = orca::WifiPrx::checkedCast(prx_);
    orcaprobe::reportResult( data, "data", ifacestring::toString( derivedPrx->getData() ) );
    return 0;
}

int 
WifiProbe::loadSubscribe( orcacm::OperationData& data )
{
    consumer_->subscribeWithString( orcaice::toString(name_) );
    orcaprobe::reportSubscribed( data, consumer_->consumerPrx()->ice_toString() );
    return 0;
}

int 
WifiProbe::loadUnsubscribe( orcacm::OperationData& data )
{
    consumer_->unsubscribeWithString( orcaice::toString(name_) );
    orcaprobe::reportUnsubscribed( data );
    return 0;
}
