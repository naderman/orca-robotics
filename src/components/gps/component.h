/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Mathew Ridley, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_GPS_COMPONENT_H
#define ORCA2_GPS_COMPONENT_H

#include <orcaice/component.h>
#include "gpsinterfaces.h"
#include <orcaiceutil/thread.h>

namespace gps {

class Driver;

class Component : public orcaice::Component
{
public:

    Component();
    virtual ~Component();

    virtual void start();
    virtual void stop();

private:

    orcaiceutil::ThreadPtr mainLoop_;
    orca::GpsDescription descr_;

    //
    // EXTERNAL INTERFACES
    //
    GpsImplPtr        gpsInterface_;
    GpsMapGridImplPtr gpsMapGridInterface_;
    GpsTimeImplPtr    gpsTimeInterface_;

    //
    // HARDWARE INTERFACES
    //
    Driver *hwDriver_;
};

}

#endif
