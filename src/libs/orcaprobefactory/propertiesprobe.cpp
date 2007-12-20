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

#include "propertiesprobe.h"

using namespace std;
using namespace orcaprobefactory;

PropertiesProbe::PropertiesProbe( const orca::FQInterfaceName & name, orcaprobe::IDisplay & display,
                                const orcaice::Context & context )
    : InterfaceProbe(name,display,context)
{
    id_ = "::orca::Properties";
    
    addOperation( "getData" );
    addOperation( "subscribe" );
    addOperation( "unsubscribe" );
}
    
int 
PropertiesProbe::loadOperationEvent( const int index, orcacm::OperationData & data )
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
PropertiesProbe::loadGetData( orcacm::OperationData & data )
{
    orca::PropertiesData result;
    try
    {
        orca::PropertiesPrx derivedPrx = orca::PropertiesPrx::checkedCast(prx_);
        result = derivedPrx->getData();
        orcaprobe::reportResult( data, "data", orcaice::toString(result) );
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
PropertiesProbe::loadSubscribe( orcacm::OperationData & data )
{
    Ice::ObjectPtr consumer = this;
    orca::PropertiesConsumerPrx callbackPrx =
            orcaice::createConsumerInterface<orca::PropertiesConsumerPrx>( context_, consumer );
    try
    {
        orca::PropertiesPrx derivedPrx = orca::PropertiesPrx::checkedCast(prx_);
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
PropertiesProbe::loadUnsubscribe( orcacm::OperationData & data )
{
    orcaprobe::reportNotImplemented( data );
    return 0;
}

void 
PropertiesProbe::setData(const orca::PropertiesData & data, const Ice::Current&)
{
    std::cout << orcaice::toString(data) << std::endl;
};
