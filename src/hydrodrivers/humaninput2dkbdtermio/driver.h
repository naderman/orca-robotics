/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_HUMANINPUT2D_KEYBOARD_TERMIO_H
#define ORCA2_HUMANINPUT2D_KEYBOARD_TERMIO_H

#include <termios.h>

#include <hydrointerfaces/humaninput2d.h>

namespace humaninput2dkbdtermio
{

class Driver : public hydrointerfaces::HumanInput2d
{
public:

    Driver( const hydroutil::Context &context );
    ~Driver();

    virtual int enable();
    virtual int disable();

    // Blocks untill new data is available
    virtual int read( Command& command );

private:

    bool enableStepInputs_;

    // obscure keyboard stuff
    int kfd_;
    struct termios cooked_;

    hydroutil::Context context_;
};

// Used for dynamically loading driver
class Factory : public hydrointerfaces::HumanInput2dFactory
{
public:
    hydrointerfaces::HumanInput2d *createDriver( 
                const hydroutil::Context &context ) const
    {
        return new Driver( context );
    }
};

} // namespace

// Used for dynamically loading driver
extern "C" {
    hydrointerfaces::HumanInput2dFactory *createHumanInput2dDriverFactory();
}

#endif
