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

Odometry2dProbe::Odometry2dProbe( const orca::FQInterfaceName& name, orcaprobe::IDisplay& display,
                                const orcaice::Context& context ) :
    InterfaceProbe(name,display,context)
{
    id_ = "::orca::Odometry2d";
    
    addOperation( "getDescription" );
    addOperation( "getData" );
    addOperation( "subscribe" );
    addOperation( "unsubscribe" );

    Ice::ObjectPtr consumer = this;
    callbackPrx_ = orcaice::createConsumerInterface<orca::Odometry2dConsumerPrx>( context_, consumer );
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
    orca::Odometry2dData result;    
    try
    {
        orca::Odometry2dPrx derivedPrx = orca::Odometry2dPrx::checkedCast(prx_);
        result = derivedPrx->getData();
        orcaprobe::reportResult( data, "data", ifacestring::toString(result) );
    }
    catch( const orca::DataNotExistException& e )
    {
        orcaprobe::reportException( data, "data is not ready on the remote interface" );
    }
    catch( const orca::HardwareFailedException& e )
    {
        orcaprobe::reportException( data, "remote hardware failure" );
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
Odometry2dProbe::loadGetDescription( orcacm::OperationData& data )
{
    orca::VehicleDescription result;
    try
    {
        orca::Odometry2dPrx derivedPrx = orca::Odometry2dPrx::checkedCast(prx_);
        result = derivedPrx->getDescription();
        orcaprobe::reportResult( data, "data", ifacestring::toString(result) );
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
Odometry2dProbe::loadSubscribe( orcacm::OperationData& data )
{
    cout<<"subscribing "<<callbackPrx_->ice_toString()<<endl;

    try
    {
        orca::Odometry2dPrx derivedPrx = orca::Odometry2dPrx::checkedCast(prx_);
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
Odometry2dProbe::loadUnsubscribe( orcacm::OperationData& data )
{
    cout<<"unsubscribing "<<callbackPrx_->ice_toString()<<endl;

    try
    {
        orca::Odometry2dPrx derivedPrx = orca::Odometry2dPrx::checkedCast(prx_);
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
Odometry2dProbe::setData(const orca::Odometry2dData& result, const Ice::Current&)
{
//     std::cout << ifacestring::toString(result) << std::endl;
    subscribeOperationData_.results.clear();
    orcaprobe::reportResult( subscribeOperationData_, "data", ifacestring::toString(result) );
    display_.setOperationData( subscribeOperationData_ );
};
