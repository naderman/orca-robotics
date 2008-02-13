/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef IMUI_H
#define IMUI_H

#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/imu.h>

// for context()
#include <orcaice/orcaice.h>

// utilities
#include <hydroiceutil/buffer.h>

// hardware driver      
#include "driver.h"

// base class
#include "insgpsi.h"


//
// Implements the Imu interface:
//     - Reads the imu messages provided by the driver and publishes them
//     - Handles all our remote calls.
//
// The component interacts with hardware and the outside
// world through the handler and (thread-safe) buffers.
//
class ImuI : public orca::Imu, public insgps::InsGpsI
{
public:
    ImuI( const orca::ImuDescription& descr,
         insgps::Driver*  hwDriver,
         const orcaice::Context & context);
    
    //
    // Imu message handler functions
    //
    
    // the handler calls this function which reads from the hwDriver_'s  buffers
    // and then publishes to the outside world   
    virtual void publish();
   
    //   
    // remote calls:
    //
    
    // Get raw imu Data
    virtual ::orca::ImuData getData(const ::Ice::Current& ) const;
    virtual ::orca::ImuDescription getDescription(const ::Ice::Current& ) const;

    // Subscribe and unsubscribe people
    virtual void subscribe(const ::orca::ImuConsumerPrx&, const ::Ice::Current& = ::Ice::Current());
    virtual void unsubscribe(const ::orca::ImuConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // Set raw imu Data
    void localSetData( const ::orca::ImuData& data );

    // Get Imu Description
    orca::ImuDescription localGetDescription() const;

private:

    // the handler will put the latest raw data into this buffer
    hydroiceutil::Buffer<orca::ImuData> imuDataBuffer_;
    
    //publishers
    orca::ImuConsumerPrx imuPublisher_;

    //topics
    IceStorm::TopicPrx topicPrx_;

    orca::ImuDescription descr_;
    
    //
    // handler stuff
    //
    
    // hardware driver
    insgps::Driver* hwDriver_;

    orca::ImuData imuData_;
   
    // read from the hwDriver_'s buffer
    void read( ::orca::ImuData& );
    
    orcaice::Context context_;
   
};


#endif
