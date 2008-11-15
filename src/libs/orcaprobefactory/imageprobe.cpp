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

#include "imageprobe.h"

using namespace std;
using namespace orcaprobefactory;

ImageProbe::ImageProbe( const orca::FQInterfaceName& name, const Ice::ObjectPrx& adminPrx, orcaprobe::AbstractDisplay& display,
                                const orcaice::Context& context )
    : InterfaceProbe(name,adminPrx,display,context)
{
    id_ = "::orca::Image";
    
    addOperation( "getData" );
    addOperation( "getDescription" );
    addOperation( "subscribe" );
    addOperation( "unsubscribe" );

    consumer_ = new orcaifaceimpl::PrintingImageConsumerImpl( context,1000,1 );
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
    orca::ImagePrx derivedPrx = orca::ImagePrx::checkedCast(prx_);
    orcaprobe::reportResult( data, "data", ifaceutil::toString( derivedPrx->getData() ) );
    return 0;
}

int 
ImageProbe::loadGetDescription( orcacm::OperationData& data )
{
    orca::ImagePrx derivedPrx = orca::ImagePrx::checkedCast(prx_);
    orcaprobe::reportResult( data, "data", ifaceutil::toString( derivedPrx->getDescription() ) );
    return 0;
}

int 
ImageProbe::loadSubscribe( orcacm::OperationData& data )
{    
    consumer_->subscribeWithString( orcaice::toString(name_) );
    orcaprobe::reportSubscribed( data, consumer_->consumerPrx()->ice_toString() );
    return 0;
}

int 
ImageProbe::loadUnsubscribe( orcacm::OperationData& data )
{    
    consumer_->unsubscribe();
    orcaprobe::reportUnsubscribed( data );
    return 0;
}
