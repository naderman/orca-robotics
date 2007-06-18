/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ROBOT2D_HARDWARE_DRIVER_H
#define ORCA2_ROBOT2D_HARDWARE_DRIVER_H

#include "types.h"

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
    virtual int read( Data& data, std::string& status )=0;

    // Writes velocity command. Returns 0 on success. Does not throw.
    virtual int write( const Command& command )=0;

    // For debugging, convert to string as much of internal state as possible
    virtual std::string toString() { return std::string(""); };

};

} // namespace

#endif
