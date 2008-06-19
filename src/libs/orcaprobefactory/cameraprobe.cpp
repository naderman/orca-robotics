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
#include <orcacm/orcacm.h>
#include <orcaprobe/orcaprobe.h>
#include <orcaifacestring/camera.h>

#include "cameraprobe.h"

using namespace std;
using namespace orcaprobefactory;

CameraProbe::CameraProbe( const orca::FQInterfaceName& name, orcaprobe::AbstractDisplay& display,
                                const orcaice::Context& context )
    : InterfaceProbe(name,display,context)
{
    id_ = "::orca::Camera";
    
    addOperation( "getData", "ImageData getData()" );
    addOperation( "getDescription", "ImageDescription getDescription()" );
    addOperation( "subscribe", "void subscribe( ImageConsumer *subscriber )");
    addOperation( "unsubscribe", "idempotent void unsubscribe( ImageConsumer *subscriber )" );

    consumer_ = new orcaifaceimpl::PrintingCameraConsumerImpl( context,1000,1 );
}
    
CameraProbe::~CameraProbe()
{
    consumer_->destroy();
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
    orca::CameraPrx derivedPrx = orca::CameraPrx::checkedCast(prx_);
    orcaprobe::reportResult( data, "data", ifacestring::toString( derivedPrx->getData() ) );
    return 0;
}

int 
CameraProbe::loadGetDescription( orcacm::OperationData& data )
{
    orca::CameraPrx derivedPrx = orca::CameraPrx::checkedCast(prx_);
    orcaprobe::reportResult( data, "data", ifacestring::toString( derivedPrx->getDescription() ) );
    return 0;
}

int 
CameraProbe::loadSubscribe( orcacm::OperationData& data )
{
    consumer_->subscribeWithString( orcaice::toString(name_) );
    orcaprobe::reportSubscribed( data, consumer_->consumerPrx()->ice_toString() );
    return 0;
}

int 
CameraProbe::loadUnsubscribe( orcacm::OperationData& data )
{
    consumer_->unsubscribeWithString( orcaice::toString(name_) );
    orcaprobe::reportUnsubscribed( data );
    return 0;
}
