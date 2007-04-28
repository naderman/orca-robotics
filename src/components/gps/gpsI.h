#if 0
/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Mathew Ridley, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_GPS_I_H
#define ORCA2_GPS_I_H

#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/gps.h>

// for context()
#include <orcaice/orcaice.h>

// utilities
#include <orcaice/ptrbuffer.h>


//
// Implements the Laser interface: Handles all our remote calls.
//
// The component interacts with this thing through the (thread-safe) buffers.
//
class GpsI : public orca::Gps
{
public:
    GpsI( const orca::GpsDescription& descr,
          const orcaice::Context & context);

    // remote calls:

    // Get GPS Data
    virtual ::orca::GpsData getData(const ::Ice::Current& ) const;

    // Get GPS Time Data
    virtual ::orca::GpsTimeData getTimeData(const ::Ice::Current& ) const;

    // Get GPS Map Grid Data
    virtual ::orca::GpsMapGridData getMapGridData(const ::Ice::Current& ) const;

    // Get Gps Geometry
    virtual ::orca::GpsDescription getDescription(const ::Ice::Current& ) const;

    // Subscribe people
    virtual void subscribe(const ::orca::GpsConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // Unsubscribe people
    virtual void unsubscribe(const ::orca::GpsConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // Subscribe people
    virtual void subscribeForTime(const ::orca::GpsTimeConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // Unsubscribe people
    virtual void unsubscribeForTime(const ::orca::GpsTimeConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // Subscribe people
    virtual void subscribeForMapGrid(const ::orca::GpsMapGridConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // Unsubscribe people
    virtual void unsubscribeForMapGrid(const ::orca::GpsMapGridConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // Set GPS Data
    void localSetData( const ::orca::GpsData& data );

    // Set GPS Time Data
    void localSetTimeData( const ::orca::GpsTimeData& data );

    // Set GPS Map Grid Data
    void localSetMapGridData( const ::orca::GpsMapGridData& data );

    // Get Gps Geometry
    orca::GpsDescription localGetDescription() const;

private:

    // the driver will put the latest data into this buffer
    orcaice::Buffer<orca::GpsData> gpsDataBuffer_;
    // the handler will put the latest data into this buffer
    orcaice::Buffer<orca::GpsMapGridData> gpsMapGridDataBuffer_;
    // the driver will put the latest data into this buffer
    orcaice::Buffer<orca::GpsTimeData> gpsTimeDataBuffer_;

    //publishers
    orca::GpsConsumerPrx gpsPublisher_;
    orca::GpsMapGridConsumerPrx gpsMapGridPublisher_;
    orca::GpsTimeConsumerPrx gpsTimePublisher_;

    //topics
    IceStorm::TopicPrx topicPrx_;
    IceStorm::TopicPrx topicMapGridPrx_;
    IceStorm::TopicPrx topicTimePrx_;

    orca::GpsDescription descr_;

    orcaice::Context context_;
};


#endif
#endif
