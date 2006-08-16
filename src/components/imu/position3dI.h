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

#ifndef ORCA2_IMU_POSITION3D_I_H
#define ORCA2_IMU_POSITION3D_I_H

#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/position3d.h>

// for context()
#include <orcaice/orcaice.h>

// utilities
#include <orcaice/ptrbuffer.h>


//
// Implements the Laser interface: Handles all our remote calls.
//
// The component interacts with this thing through the (thread-safe) buffers.
//
class Position3dI : public orca::Position3d
{
public:
    Position3dI(orca::Position3dGeometryPtr geometry,
                orcaice::Context context);

    // remote calls:

    // Get pva imu Data
    virtual ::orca::Position3dDataPtr     getData(const ::Ice::Current& ) const;

    // Get Imu Geometry
    virtual ::orca::Position3dGeometryPtr getGeometry(const ::Ice::Current& ) const;

    // Subscribe people
    virtual void subscribe(const ::orca::Position3dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // Unsubscribe people
    virtual void unsubscribe(const ::orca::Position3dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // Set pva imu Data
    void localSetData( ::orca::Position3dDataPtr data );
    
    // Get Imu Geometry
    // orca::Position3dConfigDataPtr localGetConfig() const;

private:

    // the driver will put the latest pva data into this buffer
    orcaice::PtrBuffer<orca::Position3dDataPtr> position3dDataBuffer_;

    //publishers
    orca::Position3dConsumerPrx position3dPublisher_;

    //topics
    IceStorm::TopicPrx topicPrx_;

    orca::Position3dGeometryPtr geometry_;


    orcaice::Context context_;
};


#endif
