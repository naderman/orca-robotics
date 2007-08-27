/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_INSGPS_LOCALISE3DI_H
#define ORCA2_INSGPS_LOCALISE3DI_H

#include <IceStorm/IceStorm.h>

// include provided interfaces
#include <orca/localise3d.h>

// for context()
#include <orcaice/orcaice.h>

// utilities
#include <orcaice/ptrbuffer.h>

// hardware driver      
#include "driver.h"

// base class
#include "insgpsi.h"

//
// Implements the Localise3d interface:
//     - Reads the localise3d messages provided by the driver and publishes them
//     - Handles all our remote calls.
//
// The component interacts with hardware and the outside
// world through the handler and (thread-safe) buffers.
//

class Localise3dI : public orca::Localise3d, public insgps::InsGpsI
{
public:
    Localise3dI(const orca::VehicleDescription& descr,
                insgps::Driver*  hwDriver,
                const orcaice::Context & context);

    //
    // localise3d message handler functions
    //
     
    // the handler calls this function which reads from the hwDriver_'s  buffers
    // and then publishes to the outside world   
    virtual void publish();
   
    //
    // remote calls:
    //
    
    // Get pva localise3d Data
    virtual ::orca::Localise3dData getData(const ::Ice::Current& ) const;
    virtual ::orca::Localise3dData getDataAtTime(const orca::Time&, const Ice::Current&) const;
    virtual ::orca::VehicleGeometryDescriptionPtr getVehicleGeometry(const ::Ice::Current& ) const;

    // Subscribe and unsubscribe people
    virtual void subscribe(const ::orca::Localise3dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());
    virtual void unsubscribe(const ::orca::Localise3dConsumerPrx&, const ::Ice::Current& = ::Ice::Current());

    // Set pos3d Data
    void localSetData( const ::orca::Localise3dData& data );
    
private:

    // the driver will put the latest pva data into this buffer
    orcaice::Buffer<orca::Localise3dData> localise3dDataBuffer_;

    //publishers
    orca::Localise3dConsumerPrx localise3dPublisher_;

    //topics
    IceStorm::TopicPrx topicPrx_;

    orca::VehicleDescription descr_;

    //
    // handler stuff
    //
    
    // hardware driver   
    insgps::Driver* hwDriver_;

    orca::Localise3dData localise3dData_;
    
    // read from the hwDriver_'s buffer
    void read( ::orca::Localise3dData& );
    
    orcaice::Context context_;

};

#endif
