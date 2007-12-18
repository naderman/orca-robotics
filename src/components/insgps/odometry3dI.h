/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ODOMETRY3DI_H
#define ODOMETRY3DI_H

#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/odometry3d.h>

// for context()
#include <orcaice/orcaice.h>

// utilities
#include <hydroutil/buffer.h>

// hardware driver      
#include "driver.h"

// base class
#include "insgpsi.h"

//
// Implements the Odometry3d interface:
//     - Reads the odometry3d messages provided by the driver and publishes them
//     - Handles all our remote calls.
//
// The component interacts with hardware and the outside
// world through the handler and (thread-safe) buffers.
//

class Odometry3dI : public orca::Odometry3d, public insgps::InsGpsI
{
public:
    Odometry3dI(const orca::VehicleDescription& descr,
                insgps::Driver*  hwDriver,
                const orcaice::Context & context);

    //
    // odometry3d message handler functions
    //
     
    // the handler calls this function which reads from the hwDriver_'s  buffers
    // and then publishes to the outside world   
    virtual void publish();
   
    //
    // remote calls:
    //
    
    // Get pva odometry3d Data
    virtual ::orca::Odometry3dData getData(const ::Ice::Current& ) const;
    virtual ::orca::VehicleDescription getDescription(const ::Ice::Current& ) const;

    // Subscribe and unsubscribe people
    virtual void subscribe(const ::orca::Odometry3dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());
    virtual void unsubscribe(const ::orca::Odometry3dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // Set pos3d Data
    void localSetData( const ::orca::Odometry3dData& data );
    
private:

    // the driver will put the latest pva data into this buffer
    hydroutil::Buffer<orca::Odometry3dData> odometry3dDataBuffer_;

    //publishers
    orca::Odometry3dConsumerPrx odometry3dPublisher_;

    //topics
    IceStorm::TopicPrx topicPrx_;

    orca::VehicleDescription descr_;

    //
    // handler stuff
    //
    
    // hardware driver   
    insgps::Driver* hwDriver_;

    orca::Odometry3dData odometry3dData_;
    
    // read from the hwDriver_'s buffer
    void read( ::orca::Odometry3dData& );
    
    orcaice::Context context_;

};

#endif
