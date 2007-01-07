/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TELEOP_KEYBOARD_TERMIO_DRIVER_H
#define ORCA2_TELEOP_KEYBOARD_TERMIO_DRIVER_H

#include <termio.h>

#include "../inputdriver.h"

namespace teleop
{

class KeyboardTermioDriver : public InputDriver
{
public:

    KeyboardTermioDriver( const InputDriver::Config &cfg );
    virtual ~KeyboardTermioDriver();

    virtual int enable();
    virtual int disable();

    // Blocks till new data is available
    virtual int read( orca::Velocity2dCommand& data );

private:

    orca::Velocity2dCommand command_;

    Config config_;

    double deltaSpeed_;     // [m/s]
    double deltaTurnrate_;  // [rad/sec]

    void keyboardHelp();

    // obscure keyboard stuff
    int kfd_;
    struct termio cooked_;

};

} // namespace

#endif
