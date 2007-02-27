/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcacm/orcacm.h>
#include <orcaprobe/orcaprobe.h>

#include "statusprobe.h"

using namespace std;
using namespace orcaprobefactory;

StatusProbe::StatusProbe( const orca::FQInterfaceName& name, orcaprobe::DisplayDriver& display,
                                const orcaice::Context& context )
    : InterfaceProbe(name,display,context)
{
    id_ = "::orca::Status";

    addOperation( "getData",        "nonmutating StatusData getData()" );
    addOperation( "subscribe",      "void subscribe( StatusConsumer *subscriber )" );
    addOperation( "unsubscribe",    "idempotent void unsubscribe( StatusConsumer *subscriber )" );
}
    
int 
StatusProbe::loadOperationEvent( const int index, orcacm::OperationData& data )
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
StatusProbe::loadGetData( orcacm::OperationData& data )
{
    orca::StatusData result;    
    try
    {
        orca::StatusPrx derivedPrx = orca::StatusPrx::checkedCast(prx_);
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
StatusProbe::loadSubscribe( orcacm::OperationData& data )
{
    Ice::ObjectPtr consumer = this;
    orca::StatusConsumerPrx callbackPrx = 
            orcaice::createConsumerInterface<orca::StatusConsumerPrx>( context_, consumer );
    try
    {
        orca::StatusPrx derivedPrx = orca::StatusPrx::checkedCast(prx_);
        derivedPrx->subscribe( callbackPrx );
        orcaprobe::reportSubscribed( data );
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
StatusProbe::loadUnsubscribe( orcacm::OperationData& data )
{
    orcaprobe::reportNotImplemented( data );
    return 0;
}

void 
StatusProbe::setData(const orca::StatusData& result, const Ice::Current&)
{
    std::cout << orcaice::toString(result) << std::endl;

    orcacm::OperationData data;
    // this is the result for operation "subscribe" which has user index=2;
    fillOperationData( orcaprobe::UserIndex+2, data );

    orcacm::ResultHeader res;
    res.name = "data";
    res.text = orcaice::toString(result);
    data.results.push_back( res );

    display_.setOperationData( data );
};
