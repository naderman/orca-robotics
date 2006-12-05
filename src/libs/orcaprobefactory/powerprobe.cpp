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

#include "powerprobe.h"

using namespace std;
using namespace orcaprobefactory;

PowerProbe::PowerProbe( const orca::FQInterfaceName & name, orcaprobe::DisplayDriver & display,
                                const orcaice::Context & context )
    : InterfaceProbe(name,display,context)
{
    id_ = "::orca::Power";
    
    operations_.push_back( "getData" );
    operations_.push_back( "subscribe" );
    operations_.push_back( "unsubscribe" );
}
    
int 
PowerProbe::loadOperation( const int index, orcacm::OperationData & data )
{
    //cout<<"loading home operation "<<index<<endl;

    int ret = 1;
    
    switch ( index )
    {
    case 0 :
        ret = loadGetData( data );
        break;
    case 1 :
        ret = loadSubscribe( data );
        break;
    case 2 :
        ret = loadUnsubscribe( data );
        break;
    default :
        ret = 1;
        cout<<"unknown operation index"<<endl;
        break;
    }

    return ret;
}

int 
PowerProbe::loadGetData( orcacm::OperationData & data )
{
    orca::PowerDataPtr result;
    
    try
    {
        orca::PowerPrx derivedPrx = orca::PowerPrx::checkedCast(prx_);
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

    if ( result ) {
        data.result = orcaice::toString(result);
    }
    else {
        data.result = "received empty data";
        return 1;
    }
    return 0;
}

int 
PowerProbe::loadSubscribe( orcacm::OperationData & data )
{
    Ice::ObjectPtr consumer = this;
    orca::PowerConsumerPrx callbackPrx =
            orcaice::createConsumerInterface<orca::PowerConsumerPrx>( context_, consumer );

    try
    {
        orca::PowerPrx derivedPrx = orca::PowerPrx::checkedCast(prx_);
        derivedPrx->subscribe( callbackPrx );
    }
    catch( const Ice::Exception & e )
    {
        stringstream ss;
        ss << e;
        data.result = ss.str();
        return 1;
    }
    
    data.result = "Subscribed successfully";
    return 0;
}

int 
PowerProbe::loadUnsubscribe( orcacm::OperationData & data )
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
PowerProbe::setData(const orca::PowerDataPtr& data, const Ice::Current&)
{
    std::cout << orcaice::toString(data) << std::endl;
};
