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

#include "systemstatusprobe.h"

using namespace std;
using namespace orcaprobefactory;

SystemStatusProbe::SystemStatusProbe( const orca::FQInterfaceName& name, const Ice::ObjectPrx& adminPrx, orcaprobe::AbstractDisplay& display,
                                const orcaice::Context& context ) :
    InterfaceProbe(name,adminPrx,display,context)
{
    id_ = "::orca::SystemStatus";

    addOperation( "getData",        "StatusData getData()" );
    addOperation( "subscribe",      "void subscribe( SystemStatusConsumer *subscriber )" );
    addOperation( "unsubscribe",    "idempotent void unsubscribe( SystemStatusConsumer *subscriber )" );

    consumer_ = new orcaifaceimpl::PrintingSystemStatusConsumerImpl( context,1000,1 );
}
    
int 
SystemStatusProbe::loadOperationEvent( const int index, orcacm::OperationData& data )
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
    catch( const std::exception& e )
    {
        stringstream ss;
        ss<<e.what()<<endl;
        orcaprobe::reportException( data, ss.str() );
    }
    return 0;
}

int 
SystemStatusProbe::loadGetData( orcacm::OperationData& data )
{
    orca::SystemStatusPrx derivedPrx = orca::SystemStatusPrx::checkedCast(prx_);
    orcaprobe::reportResult( data, "data", ifaceutil::toString( derivedPrx->getData() ) );
    return 0;
}

int 
SystemStatusProbe::loadSubscribe( orcacm::OperationData& data )
{
    consumer_->subscribeWithString( orcaice::toString(name_) );
    orcaprobe::reportSubscribed( data, consumer_->consumerPrx()->ice_toString() );
    return 0;
}

int 
SystemStatusProbe::loadUnsubscribe( orcacm::OperationData& data )
{
    consumer_->unsubscribe();
    orcaprobe::reportUnsubscribed( data );
    return 0;
}
