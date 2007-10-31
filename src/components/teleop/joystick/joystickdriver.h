/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TELEOP_JOYSTICK_DRIVER_H
#define ORCA2_TELEOP_JOYSTICK_DRIVER_H

#include <orcaice/context.h>
#include "../inputdriver.h"

namespace teleop
{

class Network;

class JoystickDriver : public InputDriver
{
public:

    JoystickDriver( Network* network, const orcaice::Context& context );
    virtual ~JoystickDriver();

    virtual int enable();
    virtual int disable();

    // Blocks till new data is available
    virtual int read();

private:

    Network* network_;
    orcaice::Context context_;

    // obscure joystick stuff
    int jfd_;

};

} // namespace

#endif
