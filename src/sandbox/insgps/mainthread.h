/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef MAIN_THREAD_H
#define MAIN_THREAD_H

#include <memory>
#include <hydroiceutil/hydroiceutil.h>
#include <orcaice/context.h>
#include <hydrodll/dynamicload.h>
// remote interfaces
#include <orcaifaceimpl/insImpl.h>
#include <orcaifaceimpl/gpsImpl.h>
#include <orcaifaceimpl/imuImpl.h>
// hardware interface
#include <hydrointerfaces/insgps.h>

namespace insgps {

//
// @brief the main executing loop of this insgps component.
//
class MainThread : public hydroiceutil::SubsystemThread
{

public:

    MainThread( const orcaice::Context &context );
    //virtual ~MainThread();

    // from SubsystemThread
    virtual void walk();
    // from Thread
//    virtual void stop();

private:
    // Loops until established
    void initNetworkInterface();

    // provided interfaces
    orcaifaceimpl::InsImplPtr insInterface_;
    orcaifaceimpl::GpsImplPtr gpsInterface_;
    orcaifaceimpl::ImuImplPtr imuInterface_;

    hydrointerfaces::InsGps::Config config_;

    // used to shove data from the hardware side to the network side
    // event queue, so we can put through different data types
    hydroiceutil::EventQueuePtr dataPipe_;

    // Thread talking to hardware
    hydroiceutil::SubsystemThreadPtr hwHandler_;

    orcaice::Context context_;
};

} // namespace

#endif
