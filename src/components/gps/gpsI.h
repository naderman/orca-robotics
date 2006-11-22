/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
    GpsI(orca::GpsDescriptionPtr descr,
         orcaice::Context        context);

    // remote calls:

    // Get GPS Data
    virtual ::orca::GpsDataPtr     getData(const ::Ice::Current& ) const;

    // Get GPS Time Data
    virtual ::orca::GpsTimeDataPtr getTimeData(const ::Ice::Current& ) const;

    // Get GPS Map Grid Data
    virtual ::orca::GpsMapGridDataPtr getMapGridData(const ::Ice::Current& ) const;

    // Get Gps Geometry
    virtual ::orca::GpsDescriptionPtr getDescription(const ::Ice::Current& ) const;

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
    void localSetData( ::orca::GpsDataPtr data );

    // Set GPS Time Data
    void localSetTimeData( ::orca::GpsTimeDataPtr data );

    // Set GPS Map Grid Data
    void localSetMapGridData( ::orca::GpsMapGridDataPtr data );

    // Get Gps Geometry
    orca::GpsDescriptionPtr localGetDescription() const;

private:

    // the driver will put the latest data into this buffer
    orcaice::PtrBuffer<orca::GpsDataPtr> gpsDataBuffer_;
    // the handler will put the latest data into this buffer
    orcaice::PtrBuffer<orca::GpsMapGridDataPtr> gpsMapGridDataBuffer_;
    // the driver will put the latest data into this buffer
    orcaice::PtrBuffer<orca::GpsTimeDataPtr> gpsTimeDataBuffer_;

    //publishers
    orca::GpsConsumerPrx gpsPublisher_;
    orca::GpsMapGridConsumerPrx gpsMapGridPublisher_;
    orca::GpsTimeConsumerPrx gpsTimePublisher_;

    //topics
    IceStorm::TopicPrx topicPrx_;
    IceStorm::TopicPrx topicMapGridPrx_;
    IceStorm::TopicPrx topicTimePrx_;

    orca::GpsDescriptionPtr descr_;

    orcaice::Context context_;
};


#endif
