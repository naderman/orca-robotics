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
#include <orcaifacestring/status.h>
#include <orcaifaceimpl/printingconsumers.h>

#include "statusprobe.h"

using namespace std;
using namespace orcaprobefactory;

StatusProbe::StatusProbe( const orca::FQInterfaceName& name, orcaprobe::AbstractDisplay& display,
                                const orcaice::Context& context ) :
    InterfaceProbe(name,display,context)
//     orcaifaceimpl::SimpleConsumerImpl<orca::StatusPrx,orca::StatusConsumer,orca::StatusConsumerPrx,orca::StatusData>(context)
{
    id_ = "::orca::Status";

    addOperation( "getData",        "StatusData getData()" );
    addOperation( "subscribe",      "void subscribe( StatusConsumer *subscriber )" );
    addOperation( "unsubscribe",    "idempotent void unsubscribe( StatusConsumer *subscriber )" );

    consumer_ = new orcaifaceimpl::PrintingStatusConsumerImpl( context,1000,1 );
//     consumerManager_.setConsumer( new PrintingSimpleStatusConsumerImpl( context,1000,1 ) );
}

StatusProbe::~StatusProbe()
{
    consumer_->destroy();
}
    
int 
StatusProbe::loadOperationEvent( const int index, orcacm::OperationData& data )
{    
    try
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
    catch( const Ice::Exception& e )
    {
        stringstream ss;
        ss<<e<<endl;
        orcaprobe::reportException( data, ss.str() );
    }
    catch( const std::exception& e )
    {
        stringstream ss;
        ss<<e.what()<<endl;
        orcaprobe::reportException( data, ss.str() );
    }
    return 0;
}

int 
StatusProbe::loadGetData( orcacm::OperationData& data )
{
    orca::StatusPrx derivedPrx = orca::StatusPrx::checkedCast(prx_);
    orcaprobe::reportResult( data, "data", ifacestring::toString( derivedPrx->getData() ) );
    return 0;
}

int 
StatusProbe::loadSubscribe( orcacm::OperationData& data )
{
    consumer_->subscribeWithString( orcaice::toString(name_) );
//     consumerManager_.consumer()->subscribeWithString( orcaice::toString(name_) );
    orcaprobe::reportSubscribed( data, consumer_->consumerPrx()->ice_toString() );
    return 0;
}

int 
StatusProbe::loadUnsubscribe( orcacm::OperationData& data )
{
    consumer_->unsubscribeWithString( orcaice::toString(name_) );
//     consumerManager_.consumer()->unsubscribeWithString( orcaice::toString(name_) );
    orcaprobe::reportUnsubscribed( data );
    return 0;
}
