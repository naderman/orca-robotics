/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TELEOP_VELOCITY_CONTROL2D_NETWORK_DRIVER_H
#define ORCA2_TELEOP_VELOCITY_CONTROL2D_NETWORK_DRIVER_H

#include <orca/velocitycontrol2d.h>
#include <orcaice/context.h>

#include "networkdriver.h"

namespace teleop
{

class Display;

class VelocityControl2dDriver : public NetworkDriver
{

public:
    VelocityControl2dDriver( Display* display, const orcaice::Context& context );

    virtual ~VelocityControl2dDriver();
    
    virtual int enable();
    
    virtual int disable();

    virtual void repeatCommand();

    virtual void processNewCommandIncrement( int longitudinal, int transverse, int angle );

    virtual void processNewRelativeCommand( double longitudinal, double transverse, double angle );

private:

    orca::VelocityControl2dPrx prx_;
    orca::VelocityControl2dData command_;

    double deltaSpeed_;     // [m/s]
    double deltaTurnrate_;  // [rad/sec]

    double maxSpeed_;     // [m/s]
    double maxTurnrate_;  // [rad/sec]

    Display* display_;
    orcaice::Context context_;
};

} // namespace

#endif
