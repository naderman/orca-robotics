/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include <orcaice/orcaice.h>
#include "ogmapsensormodel.h"

using namespace std;

namespace localnav {

//!
//! @author Alex Brooks
//!

OgMapSensorModel::OgMapSensorModel( const orcaice::Context&    context )
      : obsConsumer_(new orcaifaceimpl::proxiedOgMapConsumer(context)),
        obsProxy_(NULL),
        context_(context)
{
}

OgMapSensorModel::~OgMapSensorModel()
{
}


int 
OgMapSensorModel::connectToInterface()
{
    try {
        obsProxy_  = &(obsConsumer_->proxy());
        
        orcaice::connectToInterfaceWithTag<orca::OgMapPrx>( context_, obsPrx_, "Observations" );
        
        return 1;
    }
    catch( Ice::Exception &e )
    {
        stringstream ss; ss << "Error while connecting to ogmap data: " << e;
        context_.tracer()->error( ss.str() );
    }
    catch( std::exception &e )
    {
        stringstream ss; ss << "Error while connecting to ogmap data: " << e.what();
        context_.tracer()->error( ss.str() );
    }
    return 0;
}

int 
OgMapSensorModel::subscribe()
{
    try {
        obsPrx_->subscribe(  obsConsumer_->consumerPrx() );
        return 1;
    }
    catch( Ice::Exception &e )
    {
        stringstream ss; ss << "Error while subscribing to ogMap data: " << e;
        context_.tracer()->error( ss.str() );
    }
    catch( std::exception &e )
    {
        stringstream ss; ss << "Error while subscribing to ogMap data: " << e.what();
        context_.tracer()->error( ss.str() );
    }
    return 0;
}


ISensorData*
OgMapSensorModel::getNext( const int timeoutMs )
{
    orca::OgMapData ogMapData;

    int sensorRet = obsProxy_->getNext( ogMapData, timeoutMs );
    if ( sensorRet != 0 )
    {
        stringstream ss;
        ss << "Timeout waiting for ogmap data: no data for " << timeoutMs << "ms.  Stopping.";
        context_.tracer()->error( ss.str() );
        return 0;
    }
    else
    {
        ogMapSensorData_.setData( ogMapData );
        return &ogMapSensorData_;
    }
}

ISensorDescription& 
OgMapSensorModel::description()
{
    return ogMapSensorDescription_;
}

bool
OgMapSensorModel::isProxyEmpty()
{
    return obsProxy_->isEmpty();

}

    
void
OgMapSensorModel::setSimProxy( orcaice::Proxy<orca::OgMapData>*  obsProxy )
{
    obsProxy_ = obsProxy;
}


} // namespace

