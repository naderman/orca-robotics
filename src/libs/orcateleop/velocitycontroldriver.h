/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef  VELOCITY_CONTROL2D_NETWORK_DRIVER_H
#define  VELOCITY_CONTROL2D_NETWORK_DRIVER_H

#include <orca/velocitycontrol2d.h>
#include <orcaice/context.h>

#include "networkdriver.h"

namespace orcateleop
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

    virtual void processMixedCommand( double longitudinal, bool isLongIncrement, 
        double transverse, bool isTransverseIncrement, 
        double angular, bool isAngularIncrement );
    virtual void processIncrementCommand( int longitudinal, int transverse, int angular );
    virtual void processRelativeCommand( double longitudinal, double transverse, double angular );

    // May throw std::exceptions
    void enableWithString( const std::string &proxyString );
    
private:

    void sendCommand();

    orca::VelocityControl2dPrx prx_;
    orca::VelocityControl2dData command_;

    double speedIncrement_;     // [m/s]
    double turnRateIncrement_;  // [rad/sec]

    double maxSpeed_;     // [m/s]
    double maxTurnRate_;  // [rad/sec]

    Display* display_;
    orcaice::Context context_;
};

} // namespace

#endif
