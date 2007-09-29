/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_INSGPS_GPSI_H
#define ORCA2_INSGPS_GPSI_H

#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/gps.h>

// for context
#include <orcaice/orcaice.h>

// utilities
#include <orcaiceutil/buffer.h>

// hardware driver      
#include "driver.h"

// base class
#include "insgpsi.h"

//
// Implements the Gps interface:
//     - Reads the gps messages provided by the driver and publishes them
//     - Handles all our remote calls.
//
// The component interacts with hardware and the outside
// world through the handler and (thread-safe) buffers.
//
class GpsI : public orca::Gps, public insgps::InsGpsI
{
public:
    GpsI( const orca::GpsDescription& descr,
            insgps::Driver*  hwDriver,
            const orcaice::Context & context);

    //
    // gps message handler functions
    //
     
    // the handler calls this function which reads from the hwDriver_'s  buffers
    // and then publishes to the outside world   
    virtual void publish();
    
    //
    // remote calls:
    //
    
    // Get GPS Data
    virtual ::orca::GpsData getData(const ::Ice::Current& ) const;
    
    virtual ::orca::GpsDescription getDescription(const ::Ice::Current& ) const;

    // Subscribe and unsubcribe people
    virtual void subscribe(const ::orca::GpsConsumerPrx&, const ::Ice::Current& = ::Ice::Current());
    virtual void unsubscribe(const ::orca::GpsConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // Set GPS Data
    void localSetData( const ::orca::GpsData& data );

    // Get Gps Description
    orca::GpsDescription localGetDescription() const;

private:

    //
    // remote call stuff
    //      

    // the handler (this class) will put the latest data into this buffer
    orcaiceutil::Buffer<orca::GpsData> gpsDataBuffer_;

    //publishers
    orca::GpsConsumerPrx gpsPublisher_;

    //topics
    IceStorm::TopicPrx topicPrx_;

    orca::GpsDescription descr_;

    //
    // handler stuff
    //

    // hardware driver
    insgps::Driver* hwDriver_;

    orca::GpsData gpsData_;
    
    // read from the hwDriver_'s buffer
    void read( ::orca::GpsData& );
    
    orcaice::Context context_;
    
};


#endif
