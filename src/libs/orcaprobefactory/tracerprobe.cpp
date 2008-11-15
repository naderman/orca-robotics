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

#include "tracerprobe.h"

using namespace std;
using namespace orcaprobefactory;

TracerProbe::TracerProbe( const orca::FQInterfaceName& name, const Ice::ObjectPrx& adminPrx, orcaprobe::AbstractDisplay& display,
                                const orcaice::Context& context )
    : InterfaceProbe(name,adminPrx,display,context)
{
    id_ = "::orca::Tracer";

    addOperation( "getVerbosity",   "nonmutating TracerVerbosityConfig getVerbosity();" );
    addOperation( "setVerbosity",   "idempotent void setVerbosity( TracerVerbosityConfig verbosity )" );
    addOperation( "subscribe",      "void subscribe( StatusConsumer *subscriber )" );
    addOperation( "unsubscribe",    "idempotent void unsubscribe( StatusConsumer *subscriber )" );

    consumer_ = new orcaifaceimpl::PrintingTracerConsumerImpl( context,1000,1 );
}
    
int 
TracerProbe::loadOperationEvent( const int index, orcacm::OperationData& data )
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
TracerProbe::loadGetVerbosity( orcacm::OperationData& data )
{
    orca::TracerVerbosityConfig result;    
    try
    {
        orca::TracerPrx derivedPrx = orca::TracerPrx::checkedCast(prx_);
        result = derivedPrx->getVerbosity();
        stringstream ss; 
        ss  <<"error="<<result.error<<endl
            <<"warn="<<result.warning<<endl
            <<"info="<<result.info<<endl
            <<"debug="<<result.debug;
        orcaprobe::reportResult( data, "verbosity", ss.str() );
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
TracerProbe::loadSetVerbosity( orcacm::OperationData& data )
{
//     orca::TracerData result;
    orcacm::ResultHeader res;
//     
//     try
//     {
//         orca::TracerPrx derivedPrx = orca::TracerPrx::checkedCast(prx_);
//         result = derivedPrx->getData();
//     }
//     catch( const Ice::Exception& e )
//     {
//         stringstream ss;
//         ss << e;
//         res.name = "exception";
//         res.text = ss.str();
//         data.results.push_back( res );
//         return 1;
//     }

//     cout<<ifaceutil::toString(result)<<endl;
    orcaprobe::reportNotImplemented( data );
    return 0;
}

int 
TracerProbe::loadSubscribe( orcacm::OperationData& data )
{
    consumer_->subscribeWithString( orcaice::toString(name_) );
    orcaprobe::reportSubscribed( data, consumer_->consumerPrx()->ice_toString() );
    return 0;
}

int 
TracerProbe::loadUnsubscribe( orcacm::OperationData& data )
{
    consumer_->unsubscribe();
    orcaprobe::reportUnsubscribed( data );
    return 0;
}

