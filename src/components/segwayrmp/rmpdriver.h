/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef ORCA2_SEGWAY_RMP_DRIVER_INTERFACE_H
#define ORCA2_SEGWAY_RMP_DRIVER_INTERFACE_H

#include <orca/platform2d.h>
#include <orca/power.h>

#include <orcaice/context.h>

/*!

@brief Abstract interface class for Segway RMP hardware.

@author Alex Makarenko

*/
class RmpDriver
{

public:
    enum OperationalMode
    {
        TRACTOR=1,
        BALANCE,
        POWERDOWN
    };

    enum GainSchedule
    {
        NORMAL=0,
        TALL,
        HEAVY
    };

    virtual ~RmpDriver() {};
    
    virtual int enable()=0;
    virtual int disable()=0;

    //! Blocking read
    virtual int read( orca::Position2dDataPtr &position2d, orca::PowerDataPtr &power )=0;

    //! Writes velocity command
    virtual int sendMotionCommand( orca::Velocity2dCommandPtr &position2d )=0;

    virtual int setMaxVelocityScaleFactor( double scale )=0;
    virtual int setMaxTurnrateScaleFactor( double scale )=0;
    virtual int setMaxAccelerationScaleFactor( double scale )=0;
    virtual int setMaxCurrentLimitScaleFactor( double scale )=0;
    virtual int resetAllIntegrators()=0;

    virtual int setOperationalMode( OperationalMode mode )=0;
    virtual int setGainSchedule( GainSchedule sched )=0;
    virtual int enableBalanceMode( bool enable )=0;

    enum DriverType
    {
        USB_DRIVER,
        CAN_DRIVER,
        PLAYER_CLIENT_DRIVER,
        FAKE_DRIVER,
        UNKNOWN_DRIVER
    };

    struct Config
    {
        double maxSpeed;
        double maxTurnrate;
        double position2dPublishInterval;
        double powerPublishInterval;
        double statusPublishInterval;
    };
};

#endif
