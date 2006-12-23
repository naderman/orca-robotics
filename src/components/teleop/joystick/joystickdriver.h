/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TELEOP_JOYSTICK_DRIVER_H
#define ORCA2_TELEOP_JOYSTICK_DRIVER_H

#include "../inputdriver.h"

namespace teleop
{

class JoystickDriver : public InputDriver
{
public:

    JoystickDriver( const InputDriver::Config &cfg );
    virtual ~JoystickDriver();

    virtual int enable();
    virtual int disable();

    // Blocks till new data is available
    virtual int read( orca::Velocity2dCommand& data );

private:

    // returns 0 on success.  Caller should allocate
    // space for joystickDevice
    int findUSBJoystick( char *joystickDevice );

    orca::Velocity2dCommand command_;

    Config config_;

    double deltaSpeed_;     // [m/s]
    double deltaTurnrate_;  // [rad/sec]

    // obscure joystick stuff
    int jfd_;

};

} // namespace

#endif
