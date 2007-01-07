/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcacm/orcacm.h>
#include <orcaprobe/orcaprobe.h>

#include "tracerprobe.h"

using namespace std;
using namespace orcaprobefactory;

TracerProbe::TracerProbe( const orca::FQInterfaceName & name, orcaprobe::DisplayDriver & display,
                                const orcaice::Context & context )
    : InterfaceProbe(name,display,context)
{
    id_ = "::orca::Tracer";

    addOperation( "getVerbosity",   "nonmutating TracerVerbosityConfig getVerbosity();" );
    addOperation( "setVerbosity",   "idempotent void setVerbosity( TracerVerbosityConfig verbosity )" );
    addOperation( "subscribe",      "void subscribe( StatusConsumer *subscriber )" );
    addOperation( "unsubscribe",    "idempotent void unsubscribe( StatusConsumer *subscriber )" );
}
    
int 
TracerProbe::loadOperationEvent( const int index, orcacm::OperationData & data )
{    
    switch ( index )
    {
    case orcaprobe::UserIndex :
        return loadGetVerbosity( data );
    case orcaprobe::UserIndex+1 :
        return loadSetVerbosity( data );
    case orcaprobe::UserIndex+2 :
        return loadSubscribe( data );
    case orcaprobe::UserIndex+3 :
        return loadUnsubscribe( data );
    }
    return 1;
}

int 
TracerProbe::loadGetVerbosity( orcacm::OperationData & data )
{
    orca::TracerVerbosityConfig result;
    orcacm::ResultHeader res;
    
    try
    {
        orca::TracerPrx derivedPrx = orca::TracerPrx::checkedCast(prx_);
        result = derivedPrx->getVerbosity();
    }
    catch( const Ice::Exception & e )
    {
        stringstream ss;
        ss << e;
        res.name = "exception";
        res.text = ss.str();
        data.results.push_back( res );
        return 1;
    }

//     cout<<orcaice::toString(result)<<endl;
    res.name = "verbosity";
    stringstream ss; 
    ss  <<"error="<<result.error<<endl
        <<"warn="<<result.warning<<endl
        <<"info="<<result.info<<endl
        <<"debug="<<result.debug;
    res.text = ss.str();
    data.results.push_back( res );
    return 0;
}

int 
TracerProbe::loadSetVerbosity( orcacm::OperationData & data )
{
//     orca::TracerData result;
    orcacm::ResultHeader res;
//     
//     try
//     {
//         orca::TracerPrx derivedPrx = orca::TracerPrx::checkedCast(prx_);
//         result = derivedPrx->getData();
//     }
//     catch( const Ice::Exception & e )
//     {
//         stringstream ss;
//         ss << e;
//         res.name = "exception";
//         res.text = ss.str();
//         data.results.push_back( res );
//         return 1;
//     }

//     cout<<orcaice::toString(result)<<endl;
    res.name = "outcome";
    res.text = "not implemented"; //orcaice::toString(result);
    data.results.push_back( res );
    return 0;
}

int 
TracerProbe::loadSubscribe( orcacm::OperationData & data )
{
    Ice::ObjectPtr consumer = this;
    orca::TracerConsumerPrx callbackPrx = 
            orcaice::createConsumerInterface<orca::TracerConsumerPrx>( context_, consumer );

    orcacm::ResultHeader res;

    try
    {
        orca::TracerPrx derivedPrx = orca::TracerPrx::checkedCast(prx_);
        derivedPrx->subscribe( callbackPrx );
    }
    catch( const Ice::Exception & e )
    {
        stringstream ss;
        ss << e;
        res.name = "exception";
        res.text = ss.str();
        data.results.push_back( res );
        return 1;
    }
    
    res.name = "outcome";
    res.text = "Subscribed successfully";
    data.results.push_back( res );
    return 0;
}

int 
TracerProbe::loadUnsubscribe( orcacm::OperationData & data )
{
//     try
//     {
//         orca::PowerPrx derivedPrx = orca::PowerPrx::checkedCast(prx_);
// //         cout<<"unsub  "<<Ice::identityToString( consumerPrx_->ice_getIdentity() )<<endl;
//         
//         orca::PowerConsumerPrx powerConsumerPrx = orca::PowerConsumerPrx::uncheckedCast(consumerPrx_);
// //         cout<<"unsub  "<<Ice::identityToString( powerConsumerPrx->ice_getIdentity() )<<endl;
//         derivedPrx->unsubscribe( powerConsumerPrx );
//     }
//     catch( const Ice::Exception & e )
//     {
//         cout<<"caught "<<e<<endl;
//         return 1;
//     }
    
    return 0;
}

void 
TracerProbe::setData(const orca::TracerData& result, const Ice::Current&)
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
