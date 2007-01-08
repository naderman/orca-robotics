/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TELEOP_NETWORK_DRIVER_H
#define ORCA2_TELEOP_NETWORK_DRIVER_H

namespace teleop
{

class NetworkDriver
{

public:
    virtual ~NetworkDriver() {};
    
    // Returns 0 on success. Does not throw.
    virtual int enable() = 0;
    
    // Returns 0 on success. Does not throw.
    virtual int disable() = 0;

    virtual void repeatCommand() = 0;

    virtual void processNewCommandIncrement( int longitudinal, int transverse, int angle ) = 0;

    virtual void processNewRelativeCommand( double longitudinal, double transverse, double angle ) = 0;
};

} // namespace

#endif