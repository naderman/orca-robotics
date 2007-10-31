/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TELEOP_INPUT_DRIVER_H
#define ORCA2_TELEOP_INPUT_DRIVER_H

namespace teleop
{

/*

@brief Abstract interface class for human teleoperation input.

@author Alex Makarenko

*/
class InputDriver
{

public:
    virtual ~InputDriver() {};
    
    // Returns 0 on success. Does not throw.
    virtual int enable()=0;
    
    // Returns 0 on success. Does not throw.
    virtual int disable()=0;

    // Blocking read. Returns 0 on success. Does not throw.
    virtual int read()=0;
};

} // namespace

#endif
