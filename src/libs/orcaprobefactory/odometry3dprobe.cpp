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

#include "odometry3dprobe.h"

using namespace std;
using namespace orcaprobefactory;

Odometry3dProbe::Odometry3dProbe( const orca::FQInterfaceName& name, orcaprobe::IDisplay& display,
                                const orcaice::Context& context )
    : InterfaceProbe(name,display,context)
{
    id_ = "::orca::Odometry3d";
    
    addOperation( "getDescription" );
    addOperation( "getData" );
    addOperation( "subscribe" );
    addOperation( "unsubscribe" );
}
    
int 
Odometry3dProbe::loadOperationEvent( const int index, orcacm::OperationData& data )
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
Odometry3dProbe::loadGetData( orcacm::OperationData& data )
{
    orca::Odometry3dData result;
    orcacm::ResultHeader res;
    
    try
    {
        orca::Odometry3dPrx derivedPrx = orca::Odometry3dPrx::checkedCast(prx_);
        result = derivedPrx->getData();
        orcaprobe::reportResult( data, "data", orcaice::toString(result) );
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
Odometry3dProbe::loadGetDescription( orcacm::OperationData& data )
{
    orca::VehicleDescription result;
    orcacm::ResultHeader res;

    try
    {
        orca::Odometry3dPrx derivedPrx = orca::Odometry3dPrx::checkedCast(prx_);
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
Odometry3dProbe::loadSubscribe( orcacm::OperationData& data )
{
    Ice::ObjectPtr consumer = this;
    orca::Odometry3dConsumerPrx callbackPrx =
            orcaice::createConsumerInterface<orca::Odometry3dConsumerPrx>( context_, consumer );
    try
    {
        orca::Odometry3dPrx derivedPrx = orca::Odometry3dPrx::checkedCast(prx_);
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
Odometry3dProbe::loadUnsubscribe( orcacm::OperationData& data )
{
    orcaprobe::reportNotImplemented( data );
    return 0;
}

void 
Odometry3dProbe::setData(const orca::Odometry3dData& data, const Ice::Current&)
{
    std::cout << orcaice::toString(data) << std::endl;
};
