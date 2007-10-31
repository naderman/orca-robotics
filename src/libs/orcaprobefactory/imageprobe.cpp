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

#include "imageprobe.h"

using namespace std;
using namespace orcaprobefactory;

ImageProbe::ImageProbe( const orca::FQInterfaceName& name, orcaprobe::IDisplay& display,
                                const orcaice::Context& context )
    : InterfaceProbe(name,display,context)
{
    id_ = "::orca::Image";
    
    addOperation( "getData" );
    addOperation( "getDescription" );
    addOperation( "subscribe" );
    addOperation( "unsubscribe" );
}
    
int 
ImageProbe::loadOperationEvent( const int index, orcacm::OperationData& data )
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
ImageProbe::loadGetData( orcacm::OperationData& data )
{
    orca::ImageDataPtr result;
    
    try
    {
        orca::ImagePrx derivedPrx = orca::ImagePrx::checkedCast(prx_);
        result = derivedPrx->getData();
        orcaprobe::reportResult( data, "data", orcaice::toString(result) );
    }
    catch( const Ice::Exception& e )
    {
        return 1;
    }

    return 0;
}

int 
ImageProbe::loadGetDescription( orcacm::OperationData& data )
{
    orca::ImageDescription result;
    
    try
    {
        orca::ImagePrx derivedPrx = orca::ImagePrx::checkedCast(prx_);
        result = derivedPrx->getDescription();
        orcaprobe::reportResult( data, "data", orcaice::toString(result) );
    }
    catch( const Ice::Exception& e )
    {
        return 1;
    }

    return 0;
}

int 
ImageProbe::loadSubscribe( orcacm::OperationData& data )
{    
    orcaprobe::reportNotImplemented( data );
    return 0;
}

int 
ImageProbe::loadUnsubscribe( orcacm::OperationData& data )
{    
    orcaprobe::reportNotImplemented( data );
    return 0;
}
