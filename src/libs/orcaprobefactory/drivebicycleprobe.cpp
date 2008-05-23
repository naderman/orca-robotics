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

DriveBicycleProbe::DriveBicycleProbe( const orca::FQInterfaceName& name, 
                                      orcaprobe::AbstractDisplay& display,
                                      const orcaice::Context& context ) :
    InterfaceProbe(name,display,context)
{
    id_ = "::orca::DriveBicycle";
    
    addOperation( "getDescription" );
    addOperation( "getData" );
    addOperation( "subscribe" );
    addOperation( "unsubscribe" );
    addOperation( "setData" );

    Ice::ObjectPtr consumer = this;
    callbackPrx_ = orcaice::createConsumerInterface<orca::DriveBicycleConsumerPrx>( ctx_, consumer );
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
    orca::DriveBicycleData result;    
    try
    {
        orca::DriveBicyclePrx derivedPrx = orca::DriveBicyclePrx::checkedCast(prx_);
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
DriveBicycleProbe::loadGetDescription( orcacm::OperationData& data )
{
    orca::VehicleDescription result;
    try
    {
        orca::DriveBicyclePrx derivedPrx = orca::DriveBicyclePrx::checkedCast(prx_);
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
DriveBicycleProbe::loadSubscribe( orcacm::OperationData& data )
{
    cout<<"subscribing "<<callbackPrx_->ice_toString()<<endl;

    try
    {
        orca::DriveBicyclePrx derivedPrx = orca::DriveBicyclePrx::checkedCast(prx_);
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
DriveBicycleProbe::loadUnsubscribe( orcacm::OperationData& data )
{
    cout<<"unsubscribing "<<callbackPrx_->ice_toString()<<endl;

    try
    {
        orca::DriveBicyclePrx derivedPrx = orca::DriveBicyclePrx::checkedCast(prx_);
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

int 
DriveBicycleProbe::loadSetData( orcacm::OperationData& data )
{
    orcaprobe::reportNotImplemented( data );
    return 0;
}

void
DriveBicycleProbe::setData(const orca::DriveBicycleData& result, const Ice::Current&)
{
//     std::cout << ifacestring::toString(result) << std::endl;
    subscribeOperationData_.results.clear();
    orcaprobe::reportResult( subscribeOperationData_, "data", ifacestring::toString(result) );
    display_.setOperationData( subscribeOperationData_ );
};
