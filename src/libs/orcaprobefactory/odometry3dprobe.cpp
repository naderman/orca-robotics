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

#include "odometry3dprobe.h"

using namespace std;
using namespace orcaprobefactory;

Odometry3dProbe::Odometry3dProbe( const orca::FQInterfaceName & name, orcaprobe::DisplayDriver & display,
                                const orcaice::Context & context )
    : InterfaceProbe(name,display,context)
{
    id_ = "::orca::Odometry3d";
    
    addOperation( "getDescription" );
    addOperation( "getData" );
    addOperation( "subscribe" );
    addOperation( "unsubscribe" );
}
    
int 
Odometry3dProbe::loadOperationEvent( const int index, orcacm::OperationData & data )
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
Odometry3dProbe::loadGetData( orcacm::OperationData & data )
{
    orca::Odometry3dData result;
    orcacm::ResultHeader res;
    
    try
    {
        orca::Odometry3dPrx derivedPrx = orca::Odometry3dPrx::checkedCast(prx_);
        result = derivedPrx->getData();
    }
    catch( const orca::DataNotExistException & e )
    {
        cout<<"data is not ready on the remote interface"<<endl;
        return 1;
    }
    catch( const orca::HardwareFailedException & e )
    {
        cout<<"remote hardware failure"<<endl;
        return 1;
    }
    catch( const Ice::Exception & e )
    {
        cout<<"ice exception: "<<e<<endl;
        return 1;
    }

    res.name = "data";
    res.text = orcaice::toString(result);
    data.results.push_back( res );
    return 0;
}

int 
Odometry3dProbe::loadGetDescription( orcacm::OperationData & data )
{
    orca::VehicleDescription result;
    orcacm::ResultHeader res;

    try
    {
        orca::Odometry3dPrx derivedPrx = orca::Odometry3dPrx::checkedCast(prx_);
        result = derivedPrx->getDescription();
    }
    catch( const Ice::Exception & e )
    {
        cout<<"ice exception: "<<e<<endl;
        return 1;
    }

    res.name = "data";
    res.text = orcaice::toString(result);
    data.results.push_back( res );
    return 0;
}

int 
Odometry3dProbe::loadSubscribe( orcacm::OperationData & data )
{
    Ice::ObjectPtr consumer = this;
    orca::Odometry3dConsumerPrx callbackPrx =
            orcaice::createConsumerInterface<orca::Odometry3dConsumerPrx>( context_, consumer );

    orcacm::ResultHeader res;

    try
    {
        orca::Odometry3dPrx derivedPrx = orca::Odometry3dPrx::checkedCast(prx_);
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
Odometry3dProbe::loadUnsubscribe( orcacm::OperationData & data )
{
//     try
//     {
//         orca::Odometry3dPrx derivedPrx = orca::Odometry3dPrx::checkedCast(prx_);
// //         cout<<"unsub  "<<Ice::identityToString( consumerPrx_->ice_getIdentity() )<<endl;
//         
//         orca::Odometry3dConsumerPrx powerConsumerPrx = orca::Odometry3dConsumerPrx::uncheckedCast(consumerPrx_);
// //         cout<<"unsub  "<<Ice::identityToString( powerConsumerPrx->ice_getIdentity() )<<endl;
//         derivedPrx->unsubscribe( powerConsumerPrx );
//     }
//     catch( const Ice::Exception & e )
//     {
//         cout<<"caught "<<e<<endl;
//         return 1;
//     }
    
    orcacm::ResultHeader res;
    res.name = "outcome";
    res.text = "operation not implemented";
    data.results.push_back( res );
    return 0;
}

void 
Odometry3dProbe::setData(const orca::Odometry3dData & data, const Ice::Current&)
{
    std::cout << orcaice::toString(data) << std::endl;
};
