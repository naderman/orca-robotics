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

#include "laserscanner2dprobe.h"

using namespace std;
using namespace orcaprobefactory;

LaserScanner2dProbe::LaserScanner2dProbe( const orca::FQInterfaceName & name, orcaprobe::DisplayDriver & display,
                                const orcaice::Context & context )
    : InterfaceProbe(name,display,context)
{
    id_ = "::orca::LaserScanner2d";
    
    operations_.push_back( "getData" );
    operations_.push_back( "getDescription" );
    operations_.push_back( "subscribe" );
    operations_.push_back( "unsubscribe" );
}
    
int 
LaserScanner2dProbe::loadOperation( const int index, orcacm::OperationData & data )
{
    fillOperationData( index, data );

    int ret = 1;
    
    switch ( index )
    {
    case 0 :
        ret = loadGetData( data );
        break;
    case 1 :
        ret = loadGetDescription( data );
        break;
    case 2 :
        ret = loadSubscribe( data );
        break;
    case 3 :
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
LaserScanner2dProbe::loadGetData( orcacm::OperationData & data )
{
    orca::RangeScanner2dDataPtr result;
    
    try
    {
        orca::LaserScanner2dPrx derivedPrx = orca::LaserScanner2dPrx::checkedCast(prx_);
        result = derivedPrx->getData();
    }
    catch( const Ice::Exception & e )
    {
        stringstream ss;
        ss << e;
        data.result = ss.str();
        return 1;
    }

//     cout<<orcaice::toString(result)<<endl;
    data.result = orcaice::toString(result);
    return 0;
}

int 
LaserScanner2dProbe::loadGetDescription( orcacm::OperationData & data )
{
    orca::RangeScanner2dDescriptionPtr result;
    
    try
    {
        orca::LaserScanner2dPrx derivedPrx = orca::LaserScanner2dPrx::checkedCast(prx_);
        result = derivedPrx->getDescription();
    }
    catch( const Ice::Exception & e )
    {
        stringstream ss;
        ss << e;
        data.result = ss.str();
        return 1;
    }

//     cout<<orcaice::toString(result)<<endl;
    data.result = orcaice::toString(result);
    return 0;
}

int 
LaserScanner2dProbe::loadSubscribe( orcacm::OperationData & data )
{
    Ice::ObjectPtr consumer = this;
    orca::RangeScanner2dConsumerPrx callbackPrx = 
            orcaice::createConsumerInterface<orca::RangeScanner2dConsumerPrx>( context_, consumer );

    try
    {
        orca::LaserScanner2dPrx derivedPrx = orca::LaserScanner2dPrx::checkedCast(prx_);
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
LaserScanner2dProbe::loadUnsubscribe( orcacm::OperationData & data )
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
LaserScanner2dProbe::setData(const orca::RangeScanner2dDataPtr& data, const Ice::Current&)
{
    std::cout << orcaice::toString(data) << std::endl;
};
