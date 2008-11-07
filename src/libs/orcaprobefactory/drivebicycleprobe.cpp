/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcacm/orcacm.h>
#include <orcaprobe/orcaprobe.h>
#include <orcaifacestring/drivebicycle.h>

#include "drivebicycleprobe.h"

using namespace std;
using namespace orcaprobefactory;

DriveBicycleProbe::DriveBicycleProbe( const orca::FQInterfaceName& name, const Ice::ObjectPrx& adminPrx, orcaprobe::AbstractDisplay& display,
                                const orcaice::Context& context )
    : InterfaceProbe(name,adminPrx,display,context)
{
    id_ = "::orca::DriveBicycle";
    
    addOperation( "getDescription" );
    addOperation( "getData" );
    addOperation( "subscribe" );
    addOperation( "unsubscribe" );
    addOperation( "setData" );

    consumer_ = new orcaifaceimpl::PrintingDriveBicycleConsumerImpl( context,1000,1 );
}
    
DriveBicycleProbe::~DriveBicycleProbe()
{
}

int 
DriveBicycleProbe::loadOperationEvent( const int index, orcacm::OperationData& data )
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
    case orcaprobe::UserIndex+4 :
        return loadSetData( data );
    }
    return 1;
}

int 
DriveBicycleProbe::loadGetData( orcacm::OperationData& data )
{
    orca::DriveBicyclePrx derivedPrx = orca::DriveBicyclePrx::checkedCast(prx_);
    orcaprobe::reportResult( data, "data", ifacestring::toString( derivedPrx->getData() ) );
    return 0;
}

int 
DriveBicycleProbe::loadGetDescription( orcacm::OperationData& data )
{
    orca::DriveBicyclePrx derivedPrx = orca::DriveBicyclePrx::checkedCast(prx_);
    orcaprobe::reportResult( data, "data", ifacestring::toString( derivedPrx->getDescription() ) );
    return 0;
}

int 
DriveBicycleProbe::loadSubscribe( orcacm::OperationData& data )
{
    consumer_->subscribeWithString( orcaice::toString(name_) );
    orcaprobe::reportSubscribed( data, consumer_->consumerPrx()->ice_toString() );
    return 0;
}

int 
DriveBicycleProbe::loadUnsubscribe( orcacm::OperationData& data )
{
    consumer_->unsubscribeWithString( orcaice::toString(name_) );
    orcaprobe::reportUnsubscribed( data );
    return 0;
}

int 
DriveBicycleProbe::loadSetData( orcacm::OperationData& data )
{
    orcaprobe::reportNotImplemented( data );
    return 0;
}
