/*
 *  Orca-Base: Components for robotics.
 *
 *  Copyright (C) 2004
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

#ifndef ORCA2_SEGWAY_RMP_USB_DRIVER_H
#define ORCA2_SEGWAY_RMP_USB_DRIVER_H

#include "../rmpdriver.h"

// forward declarations
class CanioUsbFtdi;
class RmpUsbDataFrame;
class CanPacket;

class RmpUsbDriver : public RmpDriver
{
public:

    RmpUsbDriver();
    virtual ~RmpUsbDriver();

    virtual int enable();
    virtual int disable();

    // return 0 if everything went well, 1 otherwise
    virtual int read( orca::Position2dDataPtr &position2d, orca::PowerDataPtr &power );

    virtual int write( orca::Velocity2dCommandPtr &position2d );

private:

    // driver/hardware interface
    CanioUsbFtdi *canio_;
    RmpUsbDataFrame* dataFrame_;

    //int16_t lastSpeedX_, lastSpeedYaw_;

    // For handling rollover
    uint32_t lastRawYaw_, lastRawLeft_, lastRawRight_, lastRawForeaft_;

    // Odometry calculation
    double odomX_, odomY_, odomYaw_;

    // Flag set if motors can be enabled (i.e., enable them to be
    // enabled).  Set by a config request.
    bool motor_allow_enable;

    // Flag set if motors are currently enabled
    bool motor_enabled;

    // Maximum allowd speeds [m/s], [rad/s]
    double maxSpeed_, maxTurnrate_;

    void updateData( RmpUsbDataFrame * frame,
            orca::Position2dDataPtr &position2d, orca::PowerDataPtr &power );

    // helper to take a player command and turn it into a CAN command packet
    void makeVelocityCommandPacket( CanPacket* pkt, const orca::Velocity2dCommandPtr & command );
    void makeStatusCommand( CanPacket* pkt, uint16_t cmd, uint16_t val );
    void makeShutdownCommand( CanPacket* pkt );

    // Calculate the difference between two raw counter values, taking care
    // of rollover.
    int diff(uint32_t from, uint32_t to, bool first);


    // chip's utilities
    void WatchPacket( CanPacket* pkt, short int pktID );
    void WatchDataStream( CanPacket& pkt );

    bool firstread_;

};

#endif
