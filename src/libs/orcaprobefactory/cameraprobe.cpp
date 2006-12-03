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
#include <iomanip>    // for setw()
#include <orcaice/orcaice.h>
#include <orcacm/orcacm.h>
#include <orcaprobe/orcaprobe.h>

#include "cameraprobe.h"

using namespace std;
using namespace orcaprobefactory;

CameraProbe::CameraProbe( const orca::FQInterfaceName & name, orcaprobe::DisplayDriver & display,
                                const orcaice::Context & context )
    : InterfaceProbe(name,display,context)
{
    id_ = "::orca::Camera";
    
    operations_.push_back( "getData" );
    operations_.push_back( "getDescription" );
    operations_.push_back( "subscribe" );
    operations_.push_back( "unsubscribe" );
}
    
int 
CameraProbe::loadOperation( const int index )
{
    //cout<<"loading home operation "<<index<<endl;

    int ret = 1;
    
    switch ( index )
    {
    case 0 :
        ret = loadGetData();
        break;
    case 1 :
        ret = loadGetDescription();
        break;
    case 2 :
        ret = loadSubscribe();
        break;
    case 3 :
        ret = loadUnsubscribe();
        break;
    default :
        ret = 1;
        cout<<"unknown operation index"<<endl;
        break;
    }

    return ret;
}

int 
CameraProbe::loadGetData()
{
    orca::CameraDataPtr data;
    
    try
    {
        orca::CameraPrx derivedPrx = orca::CameraPrx::checkedCast(prx_);
        data = derivedPrx->getData();
    }
    catch( const Ice::Exception & e )
    {
        return 1;
    }

    cout<<orcaice::toString(data)<<endl;
    return 0;
}

int 
CameraProbe::loadGetDescription()
{
    orca::CameraDescriptionPtr data;
    
    try
    {
        orca::CameraPrx derivedPrx = orca::CameraPrx::checkedCast(prx_);
        data = derivedPrx->getDescription();
    }
    catch( const Ice::Exception & e )
    {
        return 1;
    }

    cout<<orcaice::toString(data)<<endl;
    return 0;
}

int 
CameraProbe::loadSubscribe()
{
//     // create the consumer only when needed
//     Ice::ObjectPtr consumer = new PowerConsumerI;
//     orca::PowerConsumerPrx powerConsumerPrx =
//             orcaice::createConsumerInterface<orca::PowerConsumerPrx>( context_, consumer );
//     consumerPrx_ = powerConsumerPrx;
//     //debug
// //     cout<<"sub  "<<Ice::identityToString( powerConsumerPrx->ice_getIdentity() )<<endl;
// //     cout<<"sub  "<<Ice::identityToString( consumerPrx_->ice_getIdentity() )<<endl;
// 
//     try
//     {
//         orca::PowerPrx derivedPrx = orca::PowerPrx::checkedCast(prx_);
//         derivedPrx->subscribe( powerConsumerPrx );
//     }
//     catch( const Ice::Exception & e )
//     {
//         return 1;
//     }
    
    return 0;
}

int 
CameraProbe::loadUnsubscribe()
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
