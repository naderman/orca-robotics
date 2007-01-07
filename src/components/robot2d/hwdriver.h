/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_SEGWAY_RMP_HARDWARE_DRIVER_H
#define ORCA2_SEGWAY_RMP_HARDWARE_DRIVER_H

#include <orca/platform2d.h>

namespace robot2d
{

/*
 * @brief Abstract interface class to robotic base.
 * @author Alex Makarenko
*/
class HwDriver
{

public:
    virtual ~HwDriver() {};
    
    // Returns 0 on success. Does not throw.
    virtual int enable()=0;
    
    // Returns 0 on success. Does not throw.
    virtual int repair()=0;
    
    // Returns 0 on success. Does not throw.
    virtual int disable()=0;

    // Blocking read. Returns 0 on success. Does not throw.
    virtual int read( orca::Position2dData &position2d, std::string &status )=0;

    // Writes velocity command. Returns 0 on success. Does not throw.
    virtual int write( const orca::Velocity2dCommand & command )=0;

    // For debugging, convert to string as much of internal state as possible
    virtual std::string toString() { return std::string(""); };

};

} // namespace

#endif
