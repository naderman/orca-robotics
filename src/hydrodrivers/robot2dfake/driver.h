/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_ROBOT2D_FAKE_H
#define HYDRO_ROBOT2D_FAKE_H

#include <hydrointerfaces/robot2d.h>

namespace robot2dfake
{

/*
    A fake driver to simplify development. Does not require any hardware.
 */
class Driver : public hydrointerfaces::Robot2d
{
public:

    Driver( const hydroutil::Context& context );
    virtual ~Driver();

    // from Robot2d
    virtual void enable();
    virtual bool read( Data& data );
    virtual void write( const Command& command );
    virtual void getStatus( std::string &status, bool &isWarn, bool &isFault );
    virtual void applyHardwareLimits( double &maxForwardSpeed, double &maxReverseSpeed, 
                              double &maxTurnrate, double &maxTurnrateAtMaxSpeed );
private:
    void disable();

    hydroutil::Context context_;
};

// Used for dynamically loading driver
class Factory : public hydrointerfaces::Robot2dFactory
{
public:
    hydrointerfaces::Robot2d *createDriver( const hydroutil::Context &context ) const
    {
        return new Driver( context );
    }
};

} // namespace

// Used for dynamically loading driver
extern "C" {
    hydrointerfaces::Robot2dFactory *createDriverFactory();
}

#endif
