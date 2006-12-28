/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_SEGWAY_RMP_FAKE_DRIVER_H
#define ORCA2_SEGWAY_RMP_FAKE_DRIVER_H

#include "hwdriver.h"
#include <orcaice/context.h>

namespace robot2d
{

/*
    A fake driver to simplify development. Does not require any hardware.
 */
class FakeDriver : public HwDriver
{
public:

    FakeDriver( const orcaice::Context& context );
    virtual ~FakeDriver();

    virtual int enable();
    virtual int repair();
    virtual int disable();

    virtual int read( orca::Position2dData& position2d, std::string & status );

    virtual int write( const orca::Velocity2dCommand& command );

private:
    orcaice::Context context_;
};

} // namespace

#endif
