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
class UsbIo;
class RmpUsbDataFrame;
class CanPacket;

class RmpUsbDriver : public RmpDriver
{
public:

    RmpUsbDriver();
    virtual ~RmpUsbDriver();

    virtual int enable();
    virtual int disable();

    virtual int read( orca::Position2dDataPtr &position2d, orca::PowerDataPtr &power );

    virtual int sendMotionCommand( orca::Velocity2dCommandPtr &position2d );

    virtual int setMaxVelocityScaleFactor( double scale );
    virtual int setMaxTurnrateScaleFactor( double scale );
    virtual int setMaxAccelerationScaleFactor( double scale );
    virtual int setMaxCurrentLimitScaleFactor( double scale );
    virtual int resetAllIntegrators();

    virtual int setOperationalMode( RmpDriver::OperationalMode mode ) { return 0; };
    virtual int setGainSchedule( RmpDriver::GainSchedule sched ) { return 0; };
    virtual int enableBalanceMode( bool enable ) { return 0; };

private:

    // driver/hardware interface
    UsbIo            *usbio_;
    RmpUsbDataFrame  *frame_;
    CanPacket        *pkt_;

    // last motion commands [segway counts]
    // used to load into status command
    int16_t lastTrans_;
    int16_t lastRot_;

    // for integrating odometry
    uint32_t lastRawYaw_;
    uint32_t lastRawForeaft_;

    // Odometry calculation
    double odomX_;
    double odomY_;
    double odomYaw_;

    // Maximum allowd speeds [m/s], [rad/s]
    double maxSpeed_;
    double maxTurnrate_;

    void integrateMotion();

    void updateData( orca::Position2dDataPtr &position2d, orca::PowerDataPtr &power );

    // helper to take a player command and turn it into a CAN command packet
    void makeMotionCommandPacket( CanPacket* pkt, const orca::Velocity2dCommandPtr & command );
    void makeStatusCommandPacket( CanPacket* pkt, uint16_t cmd, uint16_t val );
    void makeShutdownCommandPacket( CanPacket* pkt );

    // Calculate the difference between two raw counter values, taking care of rollover.
    int diff(uint32_t from, uint32_t to, bool first);
    // bullshit
    bool firstread_;

    // chip's utilities
    void watchPacket( CanPacket* pkt, short int pktID );
    void watchDataStream( CanPacket* pkt );

};

#endif
