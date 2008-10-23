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
#include <orcaifacestring/localise3d.h>

#include "localise3dprobe.h"

using namespace std;
using namespace orcaprobefactory;

Localise3dProbe::Localise3dProbe( const orca::FQInterfaceName& name, orcaprobe::AbstractDisplay& display,
                                const orcaice::Context& context )
    : InterfaceProbe(name,display,context)
{
    id_ = "::orca::Localise3d";
    
    addOperation( "getData" );
    addOperation( "getDataAtTime" );
    addOperation( "subscribe" );
    addOperation( "unsubscribe" );

    consumer_ = new orcaifaceimpl::PrintingLocalise3dConsumerImpl( context,1000,1 );
}
    
Localise3dProbe::~Localise3dProbe()
{
}

int 
Localise3dProbe::loadOperationEvent( const int index, orcacm::OperationData& data )
{
    switch ( index )
    {
    case orcaprobe::UserIndex+0 :
        return loadGetData( data );
    case orcaprobe::UserIndex+1 :
        return loadGetDataAtTime( data );
    case orcaprobe::UserIndex+2 :
        return loadSubscribe( data );
    case orcaprobe::UserIndex+3 :
        return loadUnsubscribe( data );
    }
    return 1;
}

int 
Localise3dProbe::loadGetData( orcacm::OperationData& data )
{
    orca::Localise3dPrx derivedPrx = orca::Localise3dPrx::checkedCast(prx_);
    orcaprobe::reportResult( data, "data", ifacestring::toString( derivedPrx->getData() ) );
    return 0;
}

int 
Localise3dProbe::loadGetDataAtTime( orcacm::OperationData& data )
{
    orcaprobe::reportNotImplemented( data );
    return 0;
}

int 
Localise3dProbe::loadSubscribe( orcacm::OperationData& data )
{
    consumer_->subscribeWithString( orcaice::toString(name_) );
    orcaprobe::reportSubscribed( data, consumer_->consumerPrx()->ice_toString() );
    return 0;
}

int 
Localise3dProbe::loadUnsubscribe( orcacm::OperationData& data )
{    
    consumer_->unsubscribeWithString( orcaice::toString(name_) );
    orcaprobe::reportUnsubscribed( data );
    return 0;
}
