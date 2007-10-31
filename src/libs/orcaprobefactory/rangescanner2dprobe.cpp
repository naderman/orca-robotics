/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcacm/orcacm.h>
#include <orcaprobe/orcaprobe.h>

#include "rangescanner2dprobe.h"

using namespace std;
using namespace orcaprobefactory;

RangeScanner2dProbe::RangeScanner2dProbe( const orca::FQInterfaceName& name, orcaprobe::IDisplay& display,
                                const orcaice::Context& context ) : 
    InterfaceProbe(name,display,context)
{
    id_ = "::orca::RangeScanner2d";

    addOperation( "getData",        "nonmutating RangeScanner2dData getData()" );
    addOperation( "getDescription", "nonmutating RangeScanner2dDescription getDescription()" );
    addOperation( "subscribe",      "void subscribe( RangeScanner2dConsumer *subscriber )" );
    addOperation( "unsubscribe",    "idempotent void unsubscribe( RangeScanner2dConsumer *subscriber )" );

    Ice::ObjectPtr consumer = this;
    callbackPrx_ = orcaice::createConsumerInterface<orca::RangeScanner2dConsumerPrx>( context_, consumer );
}
    
int 
RangeScanner2dProbe::loadOperationEvent( const int index, orcacm::OperationData& data )
{    
    switch ( index )
    {
    case orcaprobe::UserIndex :
        return loadGetData( data );
    case orcaprobe::UserIndex+1 :
        return loadGetDescription( data );
    case orcaprobe::UserIndex+2 :
        return loadSubscribe( data );
    case orcaprobe::UserIndex+3 :
        return loadUnsubscribe( data );
    }
    return 1;
}

int 
RangeScanner2dProbe::loadGetData( orcacm::OperationData& data )
{
    orca::RangeScanner2dDataPtr result;
    try
    {
        orca::RangeScanner2dPrx derivedPrx = orca::RangeScanner2dPrx::checkedCast(prx_);
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
RangeScanner2dProbe::loadGetDescription( orcacm::OperationData& data )
{
    orca::RangeScanner2dDescription result;
    try
    {
        orca::RangeScanner2dPrx derivedPrx = orca::RangeScanner2dPrx::checkedCast(prx_);
        result = derivedPrx->getDescription();
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
RangeScanner2dProbe::loadSubscribe( orcacm::OperationData& data )
{
    cout<<"subscribing "<<callbackPrx_->ice_toString()<<endl;
    try
    {
        orca::RangeScanner2dPrx derivedPrx = orca::RangeScanner2dPrx::checkedCast(prx_);
        derivedPrx->subscribe( callbackPrx_ );
        orcaprobe::reportSubscribed( data );

        // save the op data structure so we can use it when the data arrives
        subscribeOperationData_ = data;
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
RangeScanner2dProbe::loadUnsubscribe( orcacm::OperationData& data )
{
    cout<<"unsubscribing "<<callbackPrx_->ice_toString()<<endl;

    try
    {
        orca::RangeScanner2dPrx derivedPrx = orca::RangeScanner2dPrx::checkedCast(prx_);
        derivedPrx->unsubscribe( callbackPrx_ );
        orcaprobe::reportUnsubscribed( data );
    }
    catch( const Ice::Exception& e )
    {
        stringstream ss;
        ss<<e<<endl;
        orcaprobe::reportException( data, ss.str() );
    }
    return 0;
}

void 
RangeScanner2dProbe::setData(const orca::RangeScanner2dDataPtr& result, const Ice::Current&)
{
//     std::cout << orcaice::toString(result) << std::endl;
    subscribeOperationData_.results.clear();
    orcaprobe::reportResult( subscribeOperationData_, "data", orcaice::toString(result) );
    display_.setOperationData( subscribeOperationData_ );
};
