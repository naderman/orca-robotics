/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef MAINTHREAD_H
#define MAINTHREAD_H

// for context and thread
#include <orcaice/orcaice.h>

// hardware driver      
#include "driver.h"

// for publish()
#include "insgpsi.h"

//
// MainThread for InsGps interfaces which include Gps, Imu, and Odometry3d:
//     - Reads the gps, imu, and odometry3d messages provided by the driver and publishes them
//
// The component interacts with the driver and interfaces through (thread-safe) buffers.
//

namespace insgps{

class MainThread : public hydroiceutil::SafeThread
{
public:
    MainThread(insgps::InsGpsI&  insGpsI,
            insgps::Driver*  hwDriver,
            const orcaice::Context & context);
    
    // This is the thread's function.  It listens for data from the insgps driver,
    // and sticks it in a buffer for publishing.
    virtual void walk();
    
private:
    
    InsGpsI* insGpsI_;
           
    // hardware driver
    insgps::Driver* hwDriver_;
   
    orcaice::Context context_;
   
   
};

}

#endif
