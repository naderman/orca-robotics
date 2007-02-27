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

GpsProbe::GpsProbe( const orca::FQInterfaceName& name,
                    orcaprobe::DisplayDriver& display,
                    const orcaice::Context& context )
    : InterfaceProbe(name,display,context)
{
    id_ = "::orca::Gps";
    
    addOperation( "getDescription" );
    addOperation( "getData" );
    addOperation( "getTimeData" );
    addOperation( "getMapGridData" );
    addOperation( "subscribe" );
    addOperation( "unsubscribe" );
    addOperation( "subscribeForMapGrid" );
    addOperation( "unsubscribeForMapGrid" );
    addOperation( "subscribeForTime" );
    addOperation( "unsubscribeForTime" );

    Ice::ObjectPtr consumer = this;
    callbackGpsMapGridPrx_ =
            orcaice::createConsumerInterface<orca::GpsMapGridConsumerPrx>( context_, consumer );
}
    
int 
GpsProbe::loadOperationEvent( const int index, orcacm::OperationData& data )
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
    case orcaprobe::UserIndex+6 :
        return loadSubscribeForMapGrid( data );
    case orcaprobe::UserIndex+7 :
        return loadUnsubscribeForMapGrid( data );
    case orcaprobe::UserIndex+8 :
        return loadSubscribeForTime( data );
    case orcaprobe::UserIndex+9 :
        return loadUnsubscribeForTime( data );
    }
    return 1;
}

int 
GpsProbe::loadGetDescription( orcacm::OperationData& data )
{
    orca::GpsDescription result;
    
    try
    {
        orca::GpsPrx derivedPrx = orca::GpsPrx::checkedCast(prx_);
        result = derivedPrx->getDescription();
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
GpsProbe::loadGetData( orcacm::OperationData& data )
{
    orca::GpsData result;
    
    try
    {
        orca::GpsPrx derivedPrx = orca::GpsPrx::checkedCast(prx_);
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
GpsProbe::loadGetTimeData( orcacm::OperationData& data )
{
    orca::GpsTimeData result;
    
    try
    {
        orca::GpsPrx derivedPrx = orca::GpsPrx::checkedCast(prx_);
        result = derivedPrx->getTimeData();
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
GpsProbe::loadGetMapGridData( orcacm::OperationData& data )
{
    orca::GpsMapGridData result;
    
    try
    {
        orca::GpsPrx derivedPrx = orca::GpsPrx::checkedCast(prx_);
        result = derivedPrx->getMapGridData();
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
GpsProbe::loadSubscribe( orcacm::OperationData& data )
{
    orcaprobe::reportNotImplemented( data );
    return 0;
}

int 
GpsProbe::loadUnsubscribe( orcacm::OperationData& data )
{    
    orcaprobe::reportNotImplemented( data );
    return 0;
}

int 
GpsProbe::loadSubscribeForMapGrid( orcacm::OperationData& data )
{    
    try
    {
        orca::GpsPrx derivedPrx = orca::GpsPrx::checkedCast(prx_);
        derivedPrx->subscribeForMapGrid( callbackGpsMapGridPrx_ );
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
GpsProbe::loadUnsubscribeForMapGrid( orcacm::OperationData& data )
{    
    try
    {
        orca::GpsPrx derivedPrx = orca::GpsPrx::checkedCast(prx_);
        derivedPrx->unsubscribeForMapGrid( callbackGpsMapGridPrx_ );
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
GpsProbe::loadSubscribeForTime( orcacm::OperationData& data )
{    
    orcaprobe::reportNotImplemented( data );
    return 0;
}

int 
GpsProbe::loadUnsubscribeForTime( orcacm::OperationData& data )
{    
    orcaprobe::reportNotImplemented( data );
    return 0;
}

void 
GpsProbe::setData(const orca::GpsMapGridData& result, const Ice::Current&)
{    
    subscribeOperationData_.results.clear();
    orcaprobe::reportResult( subscribeOperationData_, "data", orcaice::toString(result) );
    display_.setOperationData( subscribeOperationData_ );
};

