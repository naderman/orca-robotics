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
#include "rangescannersensormodel.h"

using namespace std;

namespace localnav {

//!
//! @author Alex Brooks
//!

RangeScannerSensorModel::RangeScannerSensorModel( const orcaice::Context&    context )
      : obsConsumer_(new orcaifaceimpl::PtrProxiedConsumerI<orca::RangeScanner2dConsumer,orca::RangeScanner2dDataPtr>),
        obsProxy_(NULL),
        context_(context)
{
}

RangeScannerSensorModel::~RangeScannerSensorModel()
{
    if ( obsConsumer_ ) delete obsConsumer_;
}


void 
RangeScannerSensorModel::subscribeForInfo()
{
    // connect

    Ice::ObjectPtr obsConsumerPtr = obsConsumer_;
    obsConsumerPrx_ = orcaice::createConsumerInterface<orca::RangeScanner2dConsumerPrx>( context_, obsConsumerPtr );
        
    obsProxy_  = &(obsConsumer_->proxy_);

    // subscribe
    
    orca::RangeScanner2dPrx obsPrx;

    orcaice::connectToInterfaceWithTag<orca::RangeScanner2dPrx>( context_, obsPrx, "Observations" );
    obsPrx->subscribe(  obsConsumerPrx_ );
    rangeDescr_ = obsPrx->getDescription();
    
    // record the description in the RangeScannerSensorDescription class
    rangeScannerSensorDescription_.setDescr( rangeDescr_ );

    context_.tracer()->info( "Subscribed for laser" );
    

}

ISensorData*
RangeScannerSensorModel::getNext( const int timeoutMs )
{
    orca::RangeScanner2dDataPtr rangeData;

    int sensorRet = obsProxy_->getNext( rangeData, timeoutMs );
    if ( sensorRet != 0 )
    {
        stringstream ss;
        ss << "Timeout waiting for range data: no data for " << timeoutMs << "ms.  Stopping.";
        context_.tracer()->error( ss.str() );
        return 0;
    }
    else
    {
        rangeScannerSensorData_.setData( rangeData );
        return &rangeScannerSensorData_;
    }
}

ISensorDescription* 
RangeScannerSensorModel::description()
{
    return &rangeScannerSensorDescription_;
}

bool
RangeScannerSensorModel::isProxyEmpty()
{
    return obsProxy_->isEmpty();

}

    
void
RangeScannerSensorModel::setSimProxy( orcaice::PtrProxy<orca::RangeScanner2dDataPtr>*  obsProxy )
{
    obsProxy_ = obsProxy;
}


} // namespace

