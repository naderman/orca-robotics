/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Matthew Ridley, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "gpsI.h"

using namespace std;
using namespace orca;
using namespace insgps;

GpsI::GpsI(GpsDescriptionPtr   descr,
           Driver*      hwDriver,
           orcaice::Context   context )
    : InsGpsI(context),
      descr_(descr),
      hwDriver_(hwDriver),
      gpsData_(new GpsData),
      context_(context)
{
    //
    // EXTERNAL PROVIDED INTERFACE: Gps
    //
    // Find IceStorm Topic to which we'll publish
    // the main topic is 'name/*@platform/component'
    topicPrx_ = orcaice::connectToTopicWithTag<GpsConsumerPrx>
        ( context_, gpsPublisher_, "Gps" );

    //create a custom topic for the extra object
    // this topic is 'name/mapgrid@platform/component'
    topicMapGridPrx_ = orcaice::connectToTopicWithTag<GpsMapGridConsumerPrx>
        ( context_, gpsMapGridPublisher_, "Gps", "mapgrid" );

    // the main topic is 'name/*@platform/component'
    // this topic is 'name/time@platform/component'
    topicTimePrx_ = orcaice::connectToTopicWithTag<GpsTimeConsumerPrx>
        ( context_, gpsTimePublisher_, "Gps", "time" );

}

void
GpsI::publish()
{
    // blocking read with timeout (2000ms by default)
    // get the gps position
    read( gpsData_ );
            
    // send the data to icestorm and to a buffer for direct connections
    localSetData( gpsData_ );

    return;
}

//
// local calls
//

void
GpsI::read( ::orca::GpsDataPtr& data )
{
    // cout << "TRACE(gpsi::read()): in read()" << endl;
    hwDriver_->readGps( data );
    // cout << "TRACE(gpsi::read()): after read()" << endl;
}         


//
// remote calls
//

orca::GpsDataPtr
GpsI::getData(const Ice::Current& current) const
{
    std::cout << "getData()" << std::endl;

    // create a null pointer. data will be cloned into it.
    orca::GpsDataPtr data;
    // we don't need to pop the data here because we don't block on it.
    if ( gpsDataBuffer_.isEmpty() )
    {
        cout << "ERROR(gpsI.cpp): getData() called when no data had been generated!!" << endl;
        throw orca::DataNotExistException( "Gps proxy is not populated yet" );
    }else{
        gpsDataBuffer_.get( data );
    }

    return data;
}

orca::GpsMapGridDataPtr
GpsI::getMapGridData(const Ice::Current& current) const
{
    std::cout << "getMapGridData()" << std::endl;

    // create a null pointer. data will be cloned into it.
    orca::GpsMapGridDataPtr data;
    // we don't need to pop the data here because we don't block on it.
    if ( gpsMapGridDataBuffer_.isEmpty() )
    {
        cout << "ERROR(gpsI.cpp): getMapGridData() called when no data had been generated!!" << endl;
        throw orca::DataNotExistException( "Gps proxy is not populated yet" );
    }else{
        gpsMapGridDataBuffer_.get( data );
    }

    return data;
}

orca::GpsTimeDataPtr
GpsI::getTimeData(const Ice::Current& current) const
{
    std::cout << "getTimeData()" << std::endl;

    // create a null pointer. data will be cloned into it.
    orca::GpsTimeDataPtr data;
    // we don't need to pop the data here because we don't block on it.
    if ( gpsTimeDataBuffer_.isEmpty() )
    {
        cout << "ERROR(gpsI.cpp): getTimeData() called when no data had been generated!!" << endl;
        throw orca::DataNotExistException( "Gps proxy is not populated yet" );
    }else{
        gpsTimeDataBuffer_.get( data );
    }

    return data;

}

::orca::GpsDescriptionPtr
GpsI::getDescription(const ::Ice::Current& ) const
{
    std::cout << "getDescription()" << std::endl;
    return descr_;
}

::orca::GpsDescriptionPtr
GpsI::localGetDescription() const
{
    return descr_;
}

// Subscribe people
void 
GpsI::subscribe(const ::orca::GpsConsumerPrx &subscriber, const ::Ice::Current&)
{
    cout << "subscribe()" << endl;
    IceStorm::QoS qos;
    topicPrx_->subscribe( qos, subscriber );
}

// Unsubscribe people
void 
GpsI::unsubscribe(const ::orca::GpsConsumerPrx &subscriber, const ::Ice::Current&)
{
    cout << "unsubscribe()" << endl;
    topicPrx_->unsubscribe( subscriber );
}

// Subscribe people
void 
GpsI::subscribeForTime(const ::orca::GpsTimeConsumerPrx &subscriber, const ::Ice::Current&)
{
    cout << "subscribeForTime()" << endl;
    IceStorm::QoS qos;
    topicTimePrx_->subscribe( qos, subscriber );
}

// Unsubscribe people
void 
GpsI::unsubscribeForTime(const ::orca::GpsTimeConsumerPrx &subscriber, const ::Ice::Current&)
{
    cout << "unsubscribeForTime()" << endl;
    topicTimePrx_->unsubscribe( subscriber );
}

// Subscribe people
void 
GpsI::subscribeForMapGrid(const ::orca::GpsMapGridConsumerPrx &subscriber, const ::Ice::Current&)
{
    cout << "subscribeForMapGrid()" << endl;
    IceStorm::QoS qos;
    topicMapGridPrx_->subscribe( qos, subscriber );
}

// Unsubscribe people
void 
GpsI::unsubscribeForMapGrid(const ::orca::GpsMapGridConsumerPrx &subscriber, const ::Ice::Current&)
{
    cout << "unsubscribeForMapGrid()" << endl;
    topicMapGridPrx_->unsubscribe( subscriber );
}

// Set GPS Data
void
GpsI::localSetData( ::orca::GpsDataPtr data )
{
    // Stick it in the buffer so pullers can get it
    gpsDataBuffer_.push( data );

    try {
        // push it to IceStorm
        gpsPublisher_->setData( data );
    }
    catch ( Ice::ConnectionRefusedException &e )
    {
        // This could happen if IceStorm dies.
        // If we're running in an IceBox and the IceBox is shutting down,
        // this is expected (our co-located IceStorm is obviously going down).
        context_.tracer()->warning( "Failed push to IceStorm." );
    }
}

// Set GPS Time Data
void
GpsI::localSetTimeData( ::orca::GpsTimeDataPtr data )
{
    // Stick it in the buffer so pullers can get it
    gpsTimeDataBuffer_.push( data );

    try {
        // push it to IceStorm
        gpsTimePublisher_->setData( data );
    }
    catch ( Ice::ConnectionRefusedException &e )
    {
        // This could happen if IceStorm dies.
        // If we're running in an IceBox and the IceBox is shutting down,
        // this is expected (our co-located IceStorm is obviously going down).
        context_.tracer()->warning( "Failed push to IceStorm." );
    }
}

// Set GPS Map Grid Data
void
GpsI::localSetMapGridData( ::orca::GpsMapGridDataPtr data )
{
    // Stick it in the buffer so pullers can get it
    gpsMapGridDataBuffer_.push( data );

    try {
        // push it to IceStorm
        gpsMapGridPublisher_->setData( data );
    }
    catch ( Ice::ConnectionRefusedException &e )
    {
        // This could happen if IceStorm dies.
        // If we're running in an IceBox and the IceBox is shutting down,
        // this is expected (our co-located IceStorm is obviously going down).
        context_.tracer()->warning( "Failed push to IceStorm." );
    }
}


