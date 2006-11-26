/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_INSGPS_HANDLER_H
#define ORCA2_INSGPS_HANDLER_H

// #include <IceStorm/IceStorm.h>

// include provided interfaces
// #include <orca/gps.h>

// for context()
#include <orcaice/orcaice.h>

// utilities
// #include <orcaice/ptrbuffer.h>

// hardware driver      
#include "driver.h"

#include "insgpsi.h"

//
// Handler for InsGps interfaces which include Gps, Imu, and Position3d:
//     - Reads the gps messages provided by the driver and publishes them
//
// The component interacts with the driver and interfaces through (thread-safe) buffers.
//

namespace insgps{

class Handler : public orcaice::Thread
{
public:
    Handler(insgps::InsGpsI&  insGpsI,
            insgps::Driver*  hwDriver,
            orcaice::Context  context);
    
    // This is the thread's function.  It listens for data from the insgps driver,
    // and sticks it in a buffer for publishing.
    virtual void run();
    
private:
    
    InsGpsI* insGpsI_;
           
    // hardware driver
    insgps::Driver* hwDriver_;
   
    orcaice::Context context_;
   
   
};

}

#endif
