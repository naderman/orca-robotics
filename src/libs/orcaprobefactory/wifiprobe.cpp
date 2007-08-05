/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcacm/orcacm.h>
#include <orcaprobe/orcaprobe.h>

#include "wifiprobe.h"

using namespace std;
using namespace orcaprobefactory;

WifiProbe::WifiProbe( const orca::FQInterfaceName& name,
                      orcaprobe::IDisplay& display,                      
                      const orcaice::Context& context ) : 
    InterfaceProbe(name,display,context)
{
    id_ = "::orca::Wifi";

    addOperation( "getData",        "nonmutating WifiData getData()" );
    addOperation( "subscribe",      "void subscribe( WifiConsumer *subscriber )" );
    addOperation( "unsubscribe",    "idempotent void unsubscribe( WifiConsumer *subscriber )" );

    Ice::ObjectPtr consumer = this;
    callbackPrx_ = orcaice::createConsumerInterface<orca::WifiConsumerPrx>( context_, consumer );
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
    orca::WifiData result;
    try
    {
        orca::WifiPrx derivedPrx = orca::WifiPrx::checkedCast(prx_);
        result = derivedPrx->getData();
        orcaprobe::reportResult( data, "data", orcaice::toString(result) );
    }
    catch( const Ice::Exception& e )
    {
        stringstream ss;
        ss<<e<<endl;
        orcaprobe::reportException( data, ss.str() );
    }
    return 0;
}

int 
WifiProbe::loadSubscribe( orcacm::OperationData& data )
{
    cout<<"subscribing "<<callbackPrx_->ice_toString()<<endl;
    try
    {
        orca::WifiPrx derivedPrx = orca::WifiPrx::checkedCast(prx_);
        derivedPrx->subscribe( callbackPrx_ );
        orcaprobe::reportSubscribed( data );

        // save the op data structure so we can use it when the data arrives
        subscribeOperationData_ = data;
    }
    catch( const Ice::Exception& e )
    {
        stringstream ss;
        ss<<e<<endl;
        orcaprobe::reportException( data, ss.str() );
    }
    return 0;
}

int 
WifiProbe::loadUnsubscribe( orcacm::OperationData& data )
{
    cout<<"unsubscribing "<<callbackPrx_->ice_toString()<<endl;

    try
    {
        orca::WifiPrx derivedPrx = orca::WifiPrx::checkedCast(prx_);
        derivedPrx->unsubscribe( callbackPrx_ );
        orcaprobe::reportUnsubscribed( data );
    }
    catch( const Ice::Exception& e )
    {
        stringstream ss;
        ss<<e<<endl;
        orcaprobe::reportException( data, ss.str() );
    }
    return 0;
}

void 
WifiProbe::setData(const orca::WifiData& result, const Ice::Current&)
{
//     std::cout << orcaice::toString(result) << std::endl;
    subscribeOperationData_.results.clear();
    orcaprobe::reportResult( subscribeOperationData_, "data", orcaice::toString(result) );
    display_.setOperationData( subscribeOperationData_ );
};
