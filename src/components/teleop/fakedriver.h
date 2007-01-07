/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TELEOP_FAKE_INPUT_DRIVER_H
#define ORCA2_TELEOP_FAKE_INPUT_DRIVER_H

#include "inputdriver.h"

namespace teleop
{

/*
    A fake driver to simplify development. Does not require any hardware.

    @author Alex Makarenko
*/
class FakeDriver : public InputDriver
{

public:

    FakeDriver( const InputDriver::Config &cfg );
    virtual ~FakeDriver();

    virtual int enable();
    virtual int disable();

    // Blocks till new data is available
    virtual int read( orca::Velocity2dCommand& data );

private:

    Config config_;

};

} // namespace

#endif
