/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TELEOP_DISPLAY_INTERFACE_H
#define ORCA2_TELEOP_DISPLAY_INTERFACE_H

namespace teleop
{

class Display
{
public:
    virtual ~Display() {};
    
    virtual void sentNewVelocityCommand( 
                double vx, double vy, double w,
                bool vxLimit=false, bool vyLimit=false, bool wLimit=false ) = 0;

    virtual void sentNewBicycleCommand( 
                double speed, double steerAngle,
                bool speedLimit=false, bool steerAngleLimit=false ) = 0;

    virtual void sentRepeatCommand() = 0;

    virtual void failedToSendCommand() = 0;
};

} // namespace

#endif
