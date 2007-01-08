/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TELEOP_DRIVE_BICYCLE_NETWORK_DRIVER_H
#define ORCA2_TELEOP_DRIVE_BICYCLE_NETWORK_DRIVER_H

#include <orca/drivebicycle.h>
#include <orcaice/context.h>

#include "networkdriver.h"

namespace teleop
{

class Display;

class DriveBicycleDriver : public NetworkDriver
{

public:
    DriveBicycleDriver( Display* display, const orcaice::Context& context );

    virtual ~DriveBicycleDriver();
    
    virtual int enable();
    
    virtual int disable();

    virtual void repeatCommand();

    virtual void processNewCommandIncrement( int longitudinal, int transverse, int angle );

    virtual void processNewRelativeCommand( double longitudinal, double transverse, double angle );

private:

    orca::DriveBicyclePrx prx_;
    orca::DriveBicycleData command_;

    double deltaSpeed_;     // [m/s]
    double deltaSteerAngle_;  // [rad]

    double maxSpeed_;     // [m/s]
    double maxSteerAngle_;  // [rad/sec]

    Display* display_;
    orcaice::Context context_;
};

} // namespace

#endif
