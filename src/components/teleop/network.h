/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TELEOP_NETWORK_INTERFACE_H
#define ORCA2_TELEOP_NETWORK_INTERFACE_H

#define TELEOP_COMMAND_UNCHANGED 1e6

namespace teleop
{

class Network
{
public:
    virtual ~Network() {};
    
    // deltas in longitudinal, transverse directions and heading. Zero means no change.
    virtual void newCommandIncrement( int longitudinal, int transverse, int angle ) = 0;

    // relative values (%) in longitudinal, transverse  directions and heading
    virtual void newRelativeCommand( double longitudinal, double transverse, double angle ) = 0;

};

} // namespace

#endif
