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
#include <orcaifacestring/odometry2d.h>

#include "odometry2dprobe.h"

using namespace std;
using namespace orcaprobefactory;

Odometry2dProbe::Odometry2dProbe( const orca::FQInterfaceName& name, orcaprobe::AbstractDisplay& display,
                                const orcaice::Context& context ) :
    InterfaceProbe(name,display,context)
{
    id_ = "::orca::Odometry2d";
    
    addOperation( "getDescription" );
    addOperation( "getData" );
    addOperation( "subscribe" );
    addOperation( "unsubscribe" );

    consumer_ = new orcaifaceimpl::PrintingOdometry2dConsumerImpl( context,1000,1 );
}
    
Odometry2dProbe::~Odometry2dProbe()
{
    consumer_->destroy();
}

int 
Odometry2dProbe::loadOperationEvent( const int index, orcacm::OperationData& data )
{
    switch ( index )
    {
    case orcaprobe::UserIndex :
        return loadGetDescription( data );
    case orcaprobe::UserIndex+1 :
        return loadGetData( data );
    case orcaprobe::UserIndex+2 :
        return loadSubscribe( data );
    case orcaprobe::UserIndex+3 :
        return loadUnsubscribe( data );
    }
    return 1;
}

int 
Odometry2dProbe::loadGetData( orcacm::OperationData& data )
{
    orca::Odometry2dPrx derivedPrx = orca::Odometry2dPrx::checkedCast(prx_);
    orcaprobe::reportResult( data, "data", ifacestring::toString( derivedPrx->getData() ) );
    return 0;
}

int 
Odometry2dProbe::loadGetDescription( orcacm::OperationData& data )
{
    orca::Odometry2dPrx derivedPrx = orca::Odometry2dPrx::checkedCast(prx_);
    orcaprobe::reportResult( data, "data", ifacestring::toString( derivedPrx->getDescription() ) );
    return 0;
}

int 
Odometry2dProbe::loadSubscribe( orcacm::OperationData& data )
{
    consumer_->subscribeWithString( orcaice::toString(name_) );
    orcaprobe::reportSubscribed( data, consumer_->consumerPrx()->ice_toString() );
    return 0;
}

int 
Odometry2dProbe::loadUnsubscribe( orcacm::OperationData& data )
{
    consumer_->unsubscribeWithString( orcaice::toString(name_) );
    orcaprobe::reportUnsubscribed( data );
    return 0;
}
