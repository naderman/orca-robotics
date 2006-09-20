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

namespace segwayrmp
{

/*
    A fake driver to simplify development. Does not require any hardware.
 */
class FakeDriver : public HwDriver
{
public:

    FakeDriver();
    virtual ~FakeDriver();

    virtual int enable();
    virtual int repair();
    virtual int disable();

    virtual int read( orca::Position2dDataPtr &position2d, orca::PowerDataPtr &power,
              std::string & status );

    virtual int write( const orca::Velocity2dCommandPtr & command );

};

} // namespace

#endif
