/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef COMPONENT_H
#define COMPONENT_H

#include <orcaice/component.h>

#include "gpsI.h"
#include "imuI.h"
#include "odometry3dI.h"
#include "localise3dI.h"

namespace insgps
{

class MainThread;
class Driver;

class Component : public orcaice::Component
{
public:

    Component();
    virtual ~Component();

    // from orcaice::Component
    virtual void start();
    virtual void stop();

private:

    //
    // EXTERNAL INTERFACE: InsGps
    //
    GpsI* gpsObj_;
    ImuI* imuObj_;
    Odometry3dI* odometry3dObj_;
    Localise3dI* localise3dObj_;

    Ice::ObjectPtr gpsObjPtr_;
    Ice::ObjectPtr imuObjPtr_;
    Ice::ObjectPtr odometry3dObjPtr_;
    Ice::ObjectPtr localise3dObjPtr_;

    //
    // HARDWARE INTERFACES
    //

    // hardware driver
    Driver *hwDriver_;
    
    // handlers for the interfaces
    MainThread* gpsMainThread_;
    MainThread* imuMainThread_;
    MainThread* odometry3dMainThread_;
    MainThread* localise3dMainThread_;
};

} //namespace

#endif
