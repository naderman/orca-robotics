/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_SEGWAY_RMP_FAKE_DRIVER_H
#define ORCA2_SEGWAY_RMP_FAKE_DRIVER_H

#include "hwdriver.h"
#include <orcaice/context.h>

namespace segwayrmp
{

/*
    A fake driver to simplify development. Does not require any hardware.
 */
class FakeDriver : public HwDriver
{
public:

    FakeDriver( const orcaice::Context& context );

    virtual ~FakeDriver();

    virtual void enable();

    virtual void disable();

    virtual bool read( Data& data );

    virtual void write( const Command& command );

    virtual void getStatus( std::string &status, bool &isWarn, bool &isFault );

private:
    orcaice::Context context_;
};

} // namespace

#endif
