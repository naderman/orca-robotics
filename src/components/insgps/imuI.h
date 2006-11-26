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

#ifndef ORCA2_INSGPS_IMUI_H
#define ORCA2_INSGPS_IMUI_H

#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/imu.h>

// for context()
#include <orcaice/orcaice.h>

// utilities
#include <orcaice/ptrbuffer.h>

// hardware driver      
#include "driver.h"

// base class
#include "insgpsi.h"


//
// Implements the Imu interface:
//     - Reads the imu messages provided by the driver and publishes them
//     - Handles all our remote calls.
//
// The component interacts with this thing through the (thread-safe) buffers.
//
class ImuI : public orca::Imu, public insgps::InsGpsI
{
public:
    ImuI(orca::ImuDescriptionPtr descr,
         insgps::Driver*  hwDriver,
         orcaice::Context        context);
    
    //
    // Imu message handler
    //
     
    // This is the thread's function.  It listens for data from the insgps driver,
    // and sticks it in a buffer for publishing.
    // virtual void run();

    // the handler calls this function which reads from the hwDriver_'s  buffers
    // and then publishes to the outside world   
    virtual void publish();
   
    //   
    // remote calls:
    //
    
    // Get raw imu Data
    virtual ::orca::ImuDataPtr     getData(const ::Ice::Current& ) const;
    virtual ::orca::ImuDescriptionPtr getDescription(const ::Ice::Current& ) const;

    // Subscribe and unsubscribe people
    virtual void subscribe(const ::orca::ImuConsumerPrx&, const ::Ice::Current& = ::Ice::Current());
    virtual void unsubscribe(const ::orca::ImuConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // Set raw imu Data
    void localSetData( ::orca::ImuDataPtr data );

    // Get Imu Description
    orca::ImuDescriptionPtr localGetDescription() const;

private:

    // the handler will put the latest raw data into this buffer
    orcaice::PtrBuffer<orca::ImuDataPtr> imuDataBuffer_;
    
    //publishers
    orca::ImuConsumerPrx imuPublisher_;

    //topics
    IceStorm::TopicPrx topicPrx_;

    orca::ImuDescriptionPtr descr_;
    
    //
    // handler stuff
    //
    
    // hardware driver
    insgps::Driver* hwDriver_;

    orca::ImuDataPtr imuData_;
   
    // read from the hwDriver_'s buffer
    void read( ::orca::ImuDataPtr& );
    
    orcaice::Context context_;
   
};


#endif
