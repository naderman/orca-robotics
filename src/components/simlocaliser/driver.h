/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_STAGE_LOCALISER_DRIVER_H
#define ORCA2_STAGE_LOCALISER_DRIVER_H

#include <orca/localise2d.h>

namespace simlocaliser
{

class Driver
{

public:
    virtual ~Driver() {};
    
    // Returns 0 on success. Does not throw.
    virtual int enable()=0;
    
    // Returns 0 on success. Does not throw.
    virtual int repair()=0;
    
    // Returns 0 on success. Does not throw.
    virtual int disable()=0;

    // Blocking read. Returns 0 on success. Does not throw.
    virtual int read( orca::Localise2dData& position2d )=0;

    // For debugging, convert to string as much of internal state as possible
    virtual std::string toString() { return std::string(""); };

};

} // namespace

#endif
