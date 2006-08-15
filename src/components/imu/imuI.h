/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006 Ben Upcroft
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

#ifndef ORCA2_IMU_I_H
#define ORCA2_IMU_I_H

#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/imu.h>

// for context()
#include <orcaice/orcaice.h>

// utilities
#include <orcaice/ptrbuffer.h>


//
// Implements the Laser interface: Handles all our remote calls.
//
// The component interacts with this thing through the (thread-safe) buffers.
//
class ImuI : public orca::Imu
{
public:
    ImuI(orca::ImuConfigDataPtr config,
         orcaice::Context        context);

    // remote calls:

    // Get imu Data
    virtual ::orca::ImuDataPtr     getData(const ::Ice::Current& ) const;

    // Get Imu Geometry
    virtual ::orca::ImuConfigDataPtr getConfig(const ::Ice::Current& ) const;

    // Subscribe people
    virtual void subscribe(const ::orca::ImuConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // Unsubscribe people
    virtual void unsubscribe(const ::orca::ImuConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // Set imu Data
    void localSetData( ::orca::ImuDataPtr data );

    // Get Imu Geometry
    orca::ImuConfigDataPtr localGetConfig() const;

private:

    // the driver will put the latest data into this buffer
    orcaice::PtrBuffer<orca::ImuDataPtr> imuDataBuffer_;

    //publishers
    orca::ImuConsumerPrx imuPublisher_;

    //topics
    IceStorm::TopicPrx topicPrx_;

    orca::ImuConfigDataPtr config_;

    orcaice::Context context_;
};


#endif
