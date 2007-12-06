/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Mathew Ridley, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_GPS_MAIN_THREAD_H
#define ORCA2_GPS_MAIN_THREAD_H

#include <hydroutil/safethread.h>
#include <orcaice/context.h>
#include <orcaifaceimpl/gpsImpl.h>

namespace gps {

class Driver;

//
// @brief the main loop of this GPS component.
//
class MainThread : public hydroutil::SafeThread
{

public:

    MainThread( const orcaice::Context& context );
    ~MainThread();

    // from SafeThread
    virtual void walk();

private:

    orca::GpsDescription descr_;
    orcaifaceimpl::GpsImplPtr gpsInterface_;

    Driver *driver_;

    // Tries repeatedly to instantiate the driver
    void initHardwareDriver();

    // Loops until established
    void initNetworkInterface();

    void reportBogusValues( orca::GpsData        &gpsData );
    
    orcaice::Context context_;
};

} // namespace

#endif
