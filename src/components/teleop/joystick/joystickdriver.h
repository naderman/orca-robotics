/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
    virtual int read( orca::Velocity2dCommandPtr &data );

private:

    // returns 0 on success.  Caller should allocate
    // space for joystickDevice
    int findUSBJoystick( char *joystickDevice );

    orca::Velocity2dCommandPtr command_;

    Config config_;

    double deltaSpeed_;     // [m/s]
    double deltaTurnrate_;  // [rad/sec]

    // obscure joystick stuff
    int jfd_;

};

} // namespace

#endif
