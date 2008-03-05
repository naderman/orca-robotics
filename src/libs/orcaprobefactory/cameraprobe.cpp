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

#include "cameraprobe.h"

using namespace std;
using namespace orcaprobefactory;

CameraProbe::CameraProbe( const orca::FQInterfaceName& name, orcaprobe::IDisplay& display,
                                const orcaice::Context& context )
    : InterfaceProbe(name,display,context)
{
    id_ = "::orca::Camera";
    
    addOperation( "getData" );
    addOperation( "getDescription" );
    addOperation( "subscribe" );
    addOperation( "unsubscribe" );
}
    
int 
CameraProbe::loadOperationEvent( const int index, orcacm::OperationData& data )
{
    switch ( index )
    {
    case orcaprobe::UserIndex :
        return loadGetData( data );
    case orcaprobe::UserIndex+1 :
        return loadGetDescription( data );
    case orcaprobe::UserIndex+2 :
        return loadSubscribe( data );
    case orcaprobe::UserIndex+3 :
        return loadUnsubscribe( data );
    }
    return 1;
}

int 
CameraProbe::loadGetData( orcacm::OperationData& data )
{
    orca::CameraDataSequence result;
    
    try
    {
        orca::CameraPrx derivedPrx = orca::CameraPrx::checkedCast(prx_);
        result = derivedPrx->getData();
        orcaprobe::reportResult( data, "data", orcaobj::toString(result.at(0)) );
    }
    catch( const Ice::Exception& e )
    {
        return 1;
    }

    return 0;
}

int 
CameraProbe::loadGetDescription( orcacm::OperationData& data )
{
    orca::CameraDescriptionSequence result;
    
    try
    {
        orca::CameraPrx derivedPrx = orca::CameraPrx::checkedCast(prx_);
        result = derivedPrx->getDescription();
        orcaprobe::reportResult( data, "data", orcaobj::toString(result.at(0)) );
    }
    catch( const Ice::Exception& e )
    {
        return 1;
    }

    return 0;
}

int 
CameraProbe::loadSubscribe( orcacm::OperationData& data )
{    
    orcaprobe::reportNotImplemented( data );
    return 0;
}

int 
CameraProbe::loadUnsubscribe( orcacm::OperationData& data )
{    
    orcaprobe::reportNotImplemented( data );
    return 0;
}
