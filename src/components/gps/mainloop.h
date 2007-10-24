/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Mathew Ridley, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_GPS_MAINLOOP_H
#define ORCA2_GPS_MAINLOOP_H

#include <orca/gps.h>
#include <hydroutil/thread.h>
#include <orcaice/context.h>
#include <orcaifaceimpl/gpsImpl.h>

#include "driver.h"

namespace gps {

//
// @brief the main loop of this GPS component.
//
// Note: this thing self-destructs when run() returns.
//
class MainLoop : public hydroutil::Thread
{

public:

    MainLoop( orcaifaceimpl::GpsImplPtr &gpsInterface,
              Driver                    *hwDriver,
              const orca::Frame3d       &antennaOffset,
              orcaice::Context           current );
    ~MainLoop();

    virtual void run();

private:

    orcaifaceimpl::GpsImplPtr gpsInterface_;

    Driver *hwDriver_;

    orca::Frame3d antennaOffset_;

    orcaice::Context context_;
    
    void reportBogusValues( orca::GpsData        &gpsData );
    
};

}

#endif
