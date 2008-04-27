/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcacm/orcacm.h>
#include <orcaprobe/orcaprobe.h>
#include <orcaifacestring/ogmap.h>

#include "ogmapprobe.h"

using namespace std;
using namespace orcaprobefactory;

OgMapProbe::OgMapProbe( const orca::FQInterfaceName & name, 
                        orcaprobe::IDisplay & display,
                        const orcaice::Context & context )
    : InterfaceProbe(name,display,context)
{
    id_ = "::orca::OgMap";
    
    addOperation( "getData" );
    addOperation( "subscribe" );
    addOperation( "unsubscribe" );
}

int 
OgMapProbe::loadOperationEvent( const int index, orcacm::OperationData & data )
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
OgMapProbe::loadGetData( orcacm::OperationData & data )
{
    orca::OgMapData result;
    try
    {
        orca::OgMapPrx derivedPrx = orca::OgMapPrx::checkedCast(prx_);
        result = derivedPrx->getData();
        orcaprobe::reportResult( data, "data", ifacestring::toString(result) );
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
OgMapProbe::loadSubscribe( orcacm::OperationData & data )
{
    Ice::ObjectPtr consumer = this;
    orca::OgMapConsumerPrx callbackPrx =
            orcaice::createConsumerInterface<orca::OgMapConsumerPrx>( context_, consumer );
    try
    {
        orca::OgMapPrx derivedPrx = orca::OgMapPrx::checkedCast(prx_);
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
OgMapProbe::loadUnsubscribe( orcacm::OperationData & data )
{
    orcaprobe::reportNotImplemented( data );
    return 0;
}

void 
OgMapProbe::setData(const orca::OgMapData & data, const Ice::Current&)
{
    std::cout << ifacestring::toString(data) << std::endl;
};
