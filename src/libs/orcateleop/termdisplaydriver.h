/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef  TERMINAL_DISPLAY_DRIVER_H
#define  TERMINAL_DISPLAY_DRIVER_H

namespace teleop
{

class TermDisplayDriver
{

public:
    virtual ~TermDisplayDriver() {};
    
    // Returns 0 on success. Does not throw.
    virtual int enable()=0;
    
    // Returns 0 on success. Does not throw.
    virtual int disable()=0;

    virtual void show( const std::string& s ) = 0;

};

} // namespace

#endif
