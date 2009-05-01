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
#include <orcaifaceutil/binaryswitch.h>
#include "binaryswitchprobe.h"

using namespace std;
using namespace orcaprobefactory;

BinarySwitchProbe::BinarySwitchProbe( const orca::FQInterfaceName& name,
                                      const Ice::ObjectPrx&        adminPrx,
                                      orcaprobe::AbstractDisplay&  display,
                                      const orcaice::Context&      context )
    : InterfaceProbe(name,adminPrx,display,context)
{
    id_ = "::orca::BinarySwitch";
    
    addOperation( "getData" );
    addOperation( "switchOn" );
    addOperation( "switchOff" );
    addOperation( "subscribe" );
    addOperation( "unsubscribe" );

    consumer_ = new orcaifaceimpl::PrintingBinarySwitchConsumerImpl( context,1000,1 );
}

int 
BinarySwitchProbe::loadOperationEvent( const int index, orcacm::OperationData & data )
{
    switch ( index )
    {
    case orcaprobe::UserIndex :
        return loadGetData( data );
    case orcaprobe::UserIndex+1 :
        return loadSwitchOn( data );
    case orcaprobe::UserIndex+2 :
        return loadSwitchOff( data );
    case orcaprobe::UserIndex+3 :
        return loadSubscribe( data );
    case orcaprobe::UserIndex+4 :
        return loadUnsubscribe( data );
    }
    return 1;
}

int 
BinarySwitchProbe::loadGetData( orcacm::OperationData& data )
{
    orca::BinarySwitchPrx derivedPrx = orca::BinarySwitchPrx::checkedCast(prx_);
    orcaprobe::reportResult( data, "data", ifaceutil::toString( derivedPrx->getData() ) );
    return 0;
}

int 
BinarySwitchProbe::loadSwitchOn( orcacm::OperationData& data )
{
    orca::BinarySwitchPrx derivedPrx = orca::BinarySwitchPrx::checkedCast(prx_);
    orca::BinarySwitchData orcaData;
    orcaData.switchValue = true;
    derivedPrx->setData( orcaData );
    return 0;
}

int 
BinarySwitchProbe::loadSwitchOff( orcacm::OperationData& data )
{
    orca::BinarySwitchPrx derivedPrx = orca::BinarySwitchPrx::checkedCast(prx_);
    orca::BinarySwitchData orcaData;
    orcaData.switchValue = false;
    derivedPrx->setData( orcaData );
    return 0;
}

int 
BinarySwitchProbe::loadSubscribe( orcacm::OperationData& data )
{
    consumer_->subscribeWithString( orcaice::toString(name_) );
    orcaprobe::reportSubscribed( data, consumer_->consumerPrx()->ice_toString() );
    return 0;
}

int 
BinarySwitchProbe::loadUnsubscribe( orcacm::OperationData& data )
{
    consumer_->unsubscribe();
    orcaprobe::reportUnsubscribed( data );
    return 0;
}
