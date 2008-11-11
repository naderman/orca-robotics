/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcacm/orcacm.h>
#include <orcaprobe/orcaprobe.h>
#include <orcaifaceutil/localise2d.h>

#include "localise2dprobe.h"

using namespace std;
using namespace orcaprobefactory;

Localise2dProbe::Localise2dProbe( const orca::FQInterfaceName& name, const Ice::ObjectPrx& adminPrx, orcaprobe::AbstractDisplay& display,
                                const orcaice::Context& context )
    : InterfaceProbe(name,adminPrx,display,context)
{
    id_ = "::orca::Localise2d";
    
    addOperation( "getData" );
    addOperation( "getVehicleGeometry" );
    addOperation( "subscribe" );
    addOperation( "unsubscribe" );

    consumer_ = new orcaifaceimpl::PrintingLocalise2dConsumerImpl( context,1000,1 );
}
    
Localise2dProbe::~Localise2dProbe()
{
}

int 
Localise2dProbe::loadOperationEvent( const int index, orcacm::OperationData& data )
{
    switch ( index )
    {
    case orcaprobe::UserIndex+0 :
        return loadGetData( data );
    case orcaprobe::UserIndex+1 :
        return loadGetVehicleGeometry( data );
    case orcaprobe::UserIndex+2 :
        return loadSubscribe( data );
    case orcaprobe::UserIndex+3 :
        return loadUnsubscribe( data );
    }
    return 1;
}

int 
Localise2dProbe::loadGetData( orcacm::OperationData& data )
{
    orca::Localise2dPrx derivedPrx = orca::Localise2dPrx::checkedCast(prx_);
    orcaprobe::reportResult( data, "data", ifaceutil::toString( derivedPrx->getData() ) );
    return 0;
}

int 
Localise2dProbe::loadGetVehicleGeometry( orcacm::OperationData& data )
{
    orca::Localise2dPrx derivedPrx = orca::Localise2dPrx::checkedCast(prx_);
    orcaprobe::reportResult( data, "data", ifaceutil::toString( derivedPrx->getVehicleGeometry() ) );
    return 0;
}

int 
Localise2dProbe::loadSubscribe( orcacm::OperationData& data )
{
    consumer_->subscribeWithString( orcaice::toString(name_) );
    orcaprobe::reportSubscribed( data, consumer_->consumerPrx()->ice_toString() );
    return 0;
}

int 
Localise2dProbe::loadUnsubscribe( orcacm::OperationData& data )
{    
    consumer_->unsubscribeWithString( orcaice::toString(name_) );
    orcaprobe::reportUnsubscribed( data );
    return 0;
}
