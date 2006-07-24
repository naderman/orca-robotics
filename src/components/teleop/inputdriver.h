/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TELEOP_INPUT_DRIVER_H
#define ORCA2_TELEOP_INPUT_DRIVER_H

#include <orca/platform2d.h>

namespace teleop
{

/*

@brief Abstract interface class for human teleoperation input.

@author Alex Makarenko

*/
class InputDriver
{

public:
    virtual ~InputDriver() {};
    
    // Returns 0 on success. Does not throw.
    virtual int enable()=0;
    
    // Returns 0 on success. Does not throw.
    virtual int disable()=0;

    // Blocking read. Returns 0 on success. Does not throw.
    virtual int read( orca::Velocity2dCommandPtr &data )=0;

    struct Config
    {
        double maxSpeed;
        double maxTurnrate;
        std::string joystickDevice;
    };

    // Set a specifc configuration
    //virtual int setConfig( const InputDriver::Config &cfg )=0;

    // Get the current configuration
    //virtual int getConfig( InputDriver::Config &cfg )=0;

};

} // namespace

#endif
