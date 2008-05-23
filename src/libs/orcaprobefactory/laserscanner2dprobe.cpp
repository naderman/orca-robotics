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
#include <orcaifacestring/laserscanner2d.h>

#include "laserscanner2dprobe.h"

using namespace std;
using namespace orcaprobefactory;

LaserScanner2dProbe::LaserScanner2dProbe( const orca::FQInterfaceName& name, orcaprobe::AbstractDisplay& display,
                                const orcaice::Context& context ) : 
    InterfaceProbe(name,display,context)
{
    id_ = "::orca::LaserScanner2d";

    addOperation( "getData",        "RangeScanner2dData getData()" );
    addOperation( "getDescription", "RangeScanner2dDescription getDescription()" );
    addOperation( "subscribe",      "void subscribe( RangeScanner2dConsumer *subscriber )" );
    addOperation( "unsubscribe",    "idempotent void unsubscribe( RangeScanner2dConsumer *subscriber )" );

    consumer_ = new orcaifaceimpl::PrintingLaserScanner2dConsumerImpl( context,1000,1 );
}
    
LaserScanner2dProbe::~LaserScanner2dProbe()
{
    consumer_->destroy();
}

int 
LaserScanner2dProbe::loadOperationEvent( const int index, orcacm::OperationData& data )
{    
    try
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
    catch( const Ice::Exception& e )
    {
        stringstream ss;
        ss<<e<<endl;
        orcaprobe::reportException( data, ss.str() );
    }
    catch( const std::exception& e )
    {
        stringstream ss;
        ss<<e.what()<<endl;
        orcaprobe::reportException( data, ss.str() );
    }
    return 0;
}

int 
LaserScanner2dProbe::loadGetData( orcacm::OperationData& data )
{
    orca::LaserScanner2dPrx derivedPrx = orca::LaserScanner2dPrx::checkedCast(prx_);
    orcaprobe::reportResult( data, "data", ifacestring::toString( derivedPrx->getData() ) );
    return 0;
}

int 
LaserScanner2dProbe::loadGetDescription( orcacm::OperationData& data )
{
    orca::LaserScanner2dPrx derivedPrx = orca::LaserScanner2dPrx::checkedCast(prx_);
    orcaprobe::reportResult( data, "data", ifacestring::toString( derivedPrx->getDescription() ) );
    return 0;
}

int 
LaserScanner2dProbe::loadSubscribe( orcacm::OperationData& data )
{
    consumer_->subscribeWithString( orcaice::toString(name_) );
    orcaprobe::reportSubscribed( data, consumer_->consumerPrx()->ice_toString() );
    return 0;
}

int 
LaserScanner2dProbe::loadUnsubscribe( orcacm::OperationData& data )
{
    consumer_->unsubscribeWithString( orcaice::toString(name_) );
    orcaprobe::reportUnsubscribed( data );
    return 0;
}
