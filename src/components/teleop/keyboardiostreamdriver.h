/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TELEOP_KEYBOARD_IOSTREAM_DRIVER_H
#define ORCA2_TELEOP_KEYBOARD_IOSTREAM_DRIVER_H

#include "inputdriver.h"

namespace teleop
{

class Network;

class KeyboardIostreamDriver : public InputDriver
{
public:

    KeyboardIostreamDriver( Network* network );
    virtual ~KeyboardIostreamDriver();

    virtual int enable();
    virtual int disable();

    // Blocks untill new data is available
    virtual int read();

private:

    Network* network_;
};

} // namespace

#endif
