/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef  IOSTREAM_DISPLAY_DRIVER_H
#define  IOSTREAM_DISPLAY_DRIVER_H

#include "termdisplaydriver.h"

namespace teleop
{

class IostreamDisplayDriver : public TermDisplayDriver
{
public:

    // Returns 0 on success. Does not throw.
    virtual int enable();
    
    // Returns 0 on success. Does not throw.
    virtual int disable();

    virtual void show( const std::string& s );
};

} // namespace

#endif
