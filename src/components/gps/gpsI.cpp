/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Mathew Ridley, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "gpsI.h"

using namespace std;

GpsI::GpsI( const orca::GpsDescription& descr,
           const orcaice::Context & context )
    : descr_(descr),
    context_(context)
{
    //
    // EXTERNAL PROVIDED INTERFACE: Gps
    //
    // Find IceStorm Topic to which we'll publish
    // the main topic is 'name/*@platform/component'
    topicPrx_ = orcaice::connectToTopicWithTag<orca::GpsConsumerPrx>
        ( context_, gpsPublisher_, "Gps" );

    //create a custom topic for the extra object
    // this topic is 'name/mapgrid@platform/component'
    topicMapGridPrx_ = orcaice::connectToTopicWithTag<orca::GpsMapGridConsumerPrx>
        ( context_, gpsMapGridPublisher_, "Gps", "mapgrid" );

    // the main topic is 'name/*@platform/component'
    // this topic is 'name/time@platform/component'
    topicTimePrx_ = orcaice::connectToTopicWithTag<orca::GpsTimeConsumerPrx>
        ( context_, gpsTimePublisher_, "Gps", "time" );

}

orca::GpsData
GpsI::getData(const Ice::Current& current) const
{
    std::cout << "getData()" << std::endl;

    // create a null pointer. data will be cloned into it.
    orca::GpsData data;
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

orca::GpsMapGridData
GpsI::getMapGridData(const Ice::Current& current) const
{
    std::cout << "getMapGridData()" << std::endl;

    // create a null pointer. data will be cloned into it.
    orca::GpsMapGridData data;
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

orca::GpsTimeData
GpsI::getTimeData(const Ice::Current& current) const
{
    std::cout << "getTimeData()" << std::endl;

    // create a null pointer. data will be cloned into it.
    orca::GpsTimeData data;
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

// Get Laser Geometry
::orca::GpsDescription
GpsI::getDescription(const ::Ice::Current& ) const
{
    std::cout << "getDescription()" << std::endl;
    return descr_;
}

::orca::GpsDescription
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
GpsI::localSetData( const ::orca::GpsData& data )
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
GpsI::localSetTimeData( const ::orca::GpsTimeData& data )
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
GpsI::localSetMapGridData( const ::orca::GpsMapGridData& data )
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
