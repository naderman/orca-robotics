/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_INSGPS_POSITION3DI_H
#define ORCA2_INSGPS_POSITION3DI_H

#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/position3d.h>

// for context()
#include <orcaice/orcaice.h>

// utilities
#include <orcaice/ptrbuffer.h>

// hardware driver      
#include "driver.h"

//
// Implements the Position3d interface:
//     - Reads the position3d messages provided by the driver and publishes them
//     - Handles all our remote calls.
//
// The component interacts with this thing through the (thread-safe) buffers.
//

class Position3dI : public orca::Position3d, public orcaice::Thread
{
public:
    Position3dI(orca::Position3dDescriptionPtr descr,
                insgps::Driver*  hwDriver,
                orcaice::Context context);

     //
    // pos3d message handler
    //
     
    // This is the thread's function.  It listens for data from the insgps driver,
    // and sticks it in a buffer for publishing.
    virtual void run();
    
    //
    // remote calls:
    //
    
    // Get pva position3d Data
    virtual ::orca::Position3dDataPtr     getData(const ::Ice::Current& ) const;
    virtual ::orca::Position3dDescriptionPtr getDescription(const ::Ice::Current& ) const;

    // Subscribe and unsubscribe people
    virtual void subscribe(const ::orca::Position3dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());
    virtual void unsubscribe(const ::orca::Position3dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // Set pos3d Data
    void localSetData( ::orca::Position3dDataPtr data );
    
private:

    // the driver will put the latest pva data into this buffer
    orcaice::PtrBuffer<orca::Position3dDataPtr> position3dDataBuffer_;

    //publishers
    orca::Position3dConsumerPrx position3dPublisher_;

    //topics
    IceStorm::TopicPrx topicPrx_;

    orca::Position3dDescriptionPtr descr_;

    //
    // handler stuff
    //
    
    // hardware driver   
    insgps::Driver* hwDriver_;

    // read from the hwDriver_'s buffer
    void read( ::orca::Position3dDataPtr& );
    
    orcaice::Context context_;

};

#endif
