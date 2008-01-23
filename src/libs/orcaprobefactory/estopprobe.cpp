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
#include <orcaobj/orcaobj.h>
#include <orcacm/orcacm.h>
#include <orcaprobe/orcaprobe.h>

#include "estopprobe.h"

using namespace std;
using namespace orcaprobefactory;

EStopProbe::EStopProbe( const orca::FQInterfaceName & name, orcaprobe::IDisplay & display,
                                const orcaice::Context & context )
    : InterfaceProbe(name,display,context)
{
    id_ = "::orca::EStop";
    
    addOperation( "getData" );
    addOperation( "subscribe" );
    addOperation( "unsubscribe" );
}
    
int 
EStopProbe::loadOperationEvent( const int index, orcacm::OperationData & data )
{
    switch ( index )
    {
    case orcaprobe::UserIndex :
        return loadGetData( data );
    case orcaprobe::UserIndex+1 :
        return loadSubscribe( data );
    case orcaprobe::UserIndex+2 :
        return loadUnsubscribe( data );
    }
    return 1;

}

int 
EStopProbe::loadGetData( orcacm::OperationData & data )
{
    orca::EStopData result;
    try
    {
        orca::EStopPrx derivedPrx = orca::EStopPrx::checkedCast( prx_ );
        result = derivedPrx->getData();
        orcaprobe::reportResult( data, "data", orcaobj::toString(result) );
    }
    catch( const orca::DataNotExistException & e )
    {
        orcaprobe::reportException( data, "data is not ready on the remote interface" );
    }
    catch( const orca::HardwareFailedException & e )
    {
        orcaprobe::reportException( data, "remote hardware failure" );
    }
    catch( const Ice::Exception & e )
    {
        stringstream ss;
        ss<<e<<endl;
        orcaprobe::reportException( data, ss.str() );
    }
    return 0;
}

int 
EStopProbe::loadSubscribe( orcacm::OperationData & data )
{
    Ice::ObjectPtr consumer = this;
    orca::EStopConsumerPrx callbackPrx =
            orcaice::createConsumerInterface<orca::EStopConsumerPrx>( context_, consumer );
    try
    {
        orca::EStopPrx derivedPrx = orca::EStopPrx::checkedCast(prx_);
        derivedPrx->subscribe( callbackPrx );
        orcaprobe::reportSubscribed( data );
    }
    catch( const Ice::Exception & e )
    {
        stringstream ss;
        ss<<e<<endl;
        orcaprobe::reportException( data, ss.str() );
    }
    return 0;
}

int 
EStopProbe::loadUnsubscribe( orcacm::OperationData & data )
{
    orcaprobe::reportNotImplemented( data );
    return 0;
}

void 
EStopProbe::setData(const orca::EStopData & data, const Ice::Current&)
{
    std::cout << orcaobj::toString(data) << std::endl;
};
