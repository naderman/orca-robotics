/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_INSGPS_COMPONENT_H
#define ORCA2_INSGPS_COMPONENT_H

#include <orcaice/component.h>
#include <orcaice/ptrbuffer.h>
#include <orca/bros1.h>
#include <orca/imu.h>
#include <orca/position3d.h>
#include <orca/gps.h>

// implementations of Ice objects
#include "gpsI.h"
#include "imuI.h"
#include "position3dI.h"
// #include "handler.h"

namespace insgps{

class Handler;
class Driver;

class Component : public orcaice::Component
{
public:

    Component();
    virtual ~Component();

    virtual void start();
    virtual void stop();

private:

//     orca::GpsDescriptionPtr gpsDescr_;
//     orca::ImuDescriptionPtr imuDescr_;
//     orca::Position3dDescriptionPtr position3dDescr_;
    
    //
    // EXTERNAL INTERFACE: InsGps
    //
    GpsI* gpsObj_;
    ImuI* imuObj_;
    Position3dI* position3dObj_;

    Ice::ObjectPtr gpsObjPtr_;
    Ice::ObjectPtr imuObjPtr_;
    Ice::ObjectPtr position3dObjPtr_;

    //
    // HARDWARE INTERFACES
    //
    Driver *hwDriver_;
    
    Handler* gpsHandler_;
    Handler* imuHandler_;
    Handler* position3dHandler_;
};

} //namespace

#endif
