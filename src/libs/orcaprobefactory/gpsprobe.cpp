/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alexei Makarenko, Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcacm/orcacm.h>
#include <orcaprobe/orcaprobe.h>

#include "gpsprobe.h"

using namespace std;
using namespace orcaprobefactory;

GpsProbe::GpsProbe( const orca::FQInterfaceName & name,
                    orcaprobe::DisplayDriver & display,
                    const orcaice::Context & context )
    : InterfaceProbe(name,display,context)
{
    id_ = "::orca::Gps";
    
    addOperation( "getDescription" );
    addOperation( "getData" );
    addOperation( "getTimeData" );
    addOperation( "getMapGridData" );
    addOperation( "subscribe" );
    addOperation( "unsubscribe" );
//     addOperation( "subscribeForMapGrid" );
//     addOperation( "unsubscribeForMapGrid" );
//     addOperation( "subscribeForTime" );
//     addOperation( "unsubscribeForTime" );
}
    
int 
GpsProbe::loadOperationEvent( const int index, orcacm::OperationData & data )
{
    switch ( index )
    {
    case orcaprobe::UserIndex :
        return loadGetDescription( data );
    case orcaprobe::UserIndex+1 :
        return loadGetData( data );
    case orcaprobe::UserIndex+2 :
        return loadGetTimeData( data );
    case orcaprobe::UserIndex+3 :
        return loadGetMapGridData( data );
    case orcaprobe::UserIndex+4 :
        return loadSubscribe( data );
    case orcaprobe::UserIndex+5 :
        return loadUnsubscribe( data );
//     case orcaprobe::UserIndex+5 :
//         return loadSubscribeForMapGrid( data );
//     case orcaprobe::UserIndex+6 :
//         return loadUnsubscribeForMapGrid( data );
//     case orcaprobe::UserIndex+7 :
//         return loadSubscribeForTime( data );
//     case orcaprobe::UserIndex+8 :
//         return loadUnsubscribeForTime( data );
    }
    return 1;
}

int 
GpsProbe::loadGetDescription( orcacm::OperationData & data )
{
    orca::GpsDescription result;
    orcacm::ResultHeader res;
    
    try
    {
        orca::GpsPrx derivedPrx = orca::GpsPrx::checkedCast(prx_);
        result = derivedPrx->getDescription();
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
GpsProbe::loadGetData( orcacm::OperationData & data )
{
    orca::GpsData result;
    orcacm::ResultHeader res;
    
    try
    {
        orca::GpsPrx derivedPrx = orca::GpsPrx::checkedCast(prx_);
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
GpsProbe::loadGetTimeData( orcacm::OperationData & data )
{
    orca::GpsTimeData result;
    orcacm::ResultHeader res;
    
    try
    {
        orca::GpsPrx derivedPrx = orca::GpsPrx::checkedCast(prx_);
        result = derivedPrx->getTimeData();
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
GpsProbe::loadGetMapGridData( orcacm::OperationData & data )
{
    orca::GpsMapGridData result;
    orcacm::ResultHeader res;
    
    try
    {
        orca::GpsPrx derivedPrx = orca::GpsPrx::checkedCast(prx_);
        result = derivedPrx->getMapGridData();
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
GpsProbe::loadSubscribe( orcacm::OperationData & data )
{
    Ice::ObjectPtr consumer = this;
    orca::GpsConsumerPrx callbackPrx =
            orcaice::createConsumerInterface<orca::GpsConsumerPrx>( context_, consumer );

    orcacm::ResultHeader res;

    try
    {
        orca::GpsPrx derivedPrx = orca::GpsPrx::checkedCast(prx_);
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
GpsProbe::loadUnsubscribe( orcacm::OperationData & data )
{
//     try
//     {
//         orca::GpsPrx derivedPrx = orca::GpsPrx::checkedCast(prx_);
// //         cout<<"unsub  "<<Ice::identityToString( consumerPrx_->ice_getIdentity() )<<endl;
//         
//         orca::GpsConsumerPrx powerConsumerPrx = orca::GpsConsumerPrx::uncheckedCast(consumerPrx_);
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
GpsProbe::setData(const orca::GpsData & data, const Ice::Current&)
{
    std::cout << orcaice::toString(data) << std::endl;
};
