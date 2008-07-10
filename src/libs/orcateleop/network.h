/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef  NETWORK_INTERFACE_H
#define  NETWORK_INTERFACE_H

#include <hydrointerfaces/humaninput2d.h>

#define TELEOP_COMMAND_UNCHANGED 1e6

namespace teleop
{

class Network
{
public:
    virtual ~Network() {};
    
    virtual void newMixedCommand( const hydrointerfaces::HumanInput2d::Command& command ) = 0;

    // deltas in longitudinal, transverse directions and heading. Zero means no change.
    virtual void newIncrementCommand( int longitudinal, int transverse, int angle ) = 0;

    // relative values (%) in longitudinal, transverse  directions and heading
    virtual void newRelativeCommand( double longitudinal, double transverse, double angle ) = 0;

};

} // namespace

#endif
