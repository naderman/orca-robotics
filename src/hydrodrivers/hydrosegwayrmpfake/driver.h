/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_SEGWAYRMP_FAKE_H
#define HYDRO_SEGWAYRMP_FAKE_H

#include <hydrointerfaces/segwayrmp.h>

namespace segwayrmpfake
{

/*
    A fake driver to simplify development. Does not require any hardware.
 */
class Driver : public hydrointerfaces::SegwayRmp
{
public:

    Driver( const std::string &powerbaseName, const hydroutil::Context& context );
    virtual ~Driver();

    // from SegwayRmp
    virtual void enable();
    virtual void read( Data& data );
    virtual void write( const Command& command );
    virtual hydrointerfaces::SegwayRmp::Capabilities capabilities() const;
private:
    // configurable sleep interval before returning from read() [milliseconds]
    int sleepIntervalMs_;

    const std::string powerbaseName_;

    hydroutil::Context context_;
};

// Used for dynamically loading driver
class Factory : public hydrointerfaces::SegwayRmpFactory
{
public:
    hydrointerfaces::SegwayRmp *createDriver( const std::string &powerbaseName,
                                              const hydroutil::Context &context ) const
    {
        return new Driver( powerbaseName, context );
    }
};

} // namespace

// Used for dynamically loading driver
extern "C" {
    hydrointerfaces::SegwayRmpFactory *createSegwayRmpDriverFactory();
}

#endif
