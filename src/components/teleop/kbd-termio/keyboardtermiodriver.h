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

class Network;

class KeyboardTermioDriver : public InputDriver
{
public:

    KeyboardTermioDriver( Network* network, bool enableStepInputs );
    virtual ~KeyboardTermioDriver();

    virtual int enable();
    virtual int disable();

    // Blocks untill new data is available
    virtual int read();

private:

    Network* network_;
    bool enableStepInputs_;

    // obscure keyboard stuff
    int kfd_;
    struct termio cooked_;

};

} // namespace

#endif
