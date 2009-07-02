/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcacm/orcacm.h>
#include <orcaprobe/orcaprobe.h>

#include "estopprobe.h"

using namespace std;
using namespace orcaprobefactory;

EStopProbe::EStopProbe( const orca::FQInterfaceName& name, const Ice::ObjectPrx& adminPrx, orcaprobe::AbstractDisplay& display,
                                const orcaice::Context & context )
    : InterfaceProbe(name,adminPrx,display,context)
{
    id_ = "::orca::EStop";
    
    addOperation( "getData" );
    addOperation( "setEnabled" );
    addOperation( "setDisabled" );
    addOperation( "subscribe" );
    addOperation( "unsubscribe" );

    consumer_ = new orcaifaceimpl::PrintingEStopConsumerImpl( context,1000,1 );
}

int 
EStopProbe::loadOperationEvent( const int index, orcacm::OperationData & data )
{
    switch ( index )
    {
    case orcaprobe::UserIndex+0 :
        return loadGetData( data );
    case orcaprobe::UserIndex+1 :
        return loadSetEnabled( data );
    case orcaprobe::UserIndex+2 :
        return loadSetDisabled( data );
    case orcaprobe::UserIndex+3 :
        return loadSubscribe( data );
    case orcaprobe::UserIndex+4 :
        return loadUnsubscribe( data );
    }
    return 1;
}

int 
EStopProbe::loadGetData( orcacm::OperationData & data )
{
    orca::EStopPrx derivedPrx = orca::EStopPrx::checkedCast(prx_);
    orcaprobe::reportResult( data, "data", ifaceutil::toString( derivedPrx->getData() ) );
    return 0;
}

int 
EStopProbe::loadSetEnabled( orcacm::OperationData & data )
{
    orca::EStopPrx derivedPrx = orca::EStopPrx::checkedCast(prx_);
    derivedPrx->setEnabled( true );
    return 0;
}

int 
EStopProbe::loadSetDisabled( orcacm::OperationData & data )
{
    orca::EStopPrx derivedPrx = orca::EStopPrx::checkedCast(prx_);
    derivedPrx->setEnabled( false );
    return 0;
}

int 
EStopProbe::loadSubscribe( orcacm::OperationData & data )
{
    consumer_->subscribeWithString( orcaice::toString(name_) );
    orcaprobe::reportSubscribed( data, consumer_->consumerPrx()->ice_toString() );
    return 0;
}

int 
EStopProbe::loadUnsubscribe( orcacm::OperationData & data )
{
    consumer_->unsubscribe();
    orcaprobe::reportUnsubscribed( data );
    return 0;
}
