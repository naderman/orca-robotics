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

#include "position3dprobe.h"

using namespace std;
using namespace orcaprobefactory;

Position3dProbe::Position3dProbe( const orca::FQInterfaceName& name, orcaprobe::DisplayDriver& display,
                                const orcaice::Context& context )
    : InterfaceProbe(name,display,context)
{
    id_ = "::orca::Position3d";
    
    addOperation( "getDescription" );
    addOperation( "getData" );
    addOperation( "subscribe" );
    addOperation( "unsubscribe" );
}
    
int 
Position3dProbe::loadOperationEvent( const int index, orcacm::OperationData& data )
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
Position3dProbe::loadGetData( orcacm::OperationData& data )
{
    orca::Position3dData result;
    orcacm::ResultHeader res;
    
    try
    {
        orca::Position3dPrx derivedPrx = orca::Position3dPrx::checkedCast(prx_);
        result = derivedPrx->getData();
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
Position3dProbe::loadGetDescription( orcacm::OperationData& data )
{
    orcaprobe::reportNotImplemented( data );
    return 0;
}

int 
Position3dProbe::loadSubscribe( orcacm::OperationData& data )
{
    Ice::ObjectPtr consumer = this;
    orca::Position3dConsumerPrx callbackPrx =
            orcaice::createConsumerInterface<orca::Position3dConsumerPrx>( context_, consumer );
    try
    {
        orca::Position3dPrx derivedPrx = orca::Position3dPrx::checkedCast(prx_);
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
Position3dProbe::loadUnsubscribe( orcacm::OperationData& data )
{
    orcaprobe::reportNotImplemented( data );
    return 0;
}

void 
Position3dProbe::setData(const orca::Position3dData& data, const Ice::Current&)
{
    std::cout << orcaice::toString(data) << std::endl;
};
