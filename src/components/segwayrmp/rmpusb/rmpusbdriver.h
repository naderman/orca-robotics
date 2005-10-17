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

#include "rmpdrivercontext.h"
#include <orcaiceutil/thread.h>
#include <orcaiceutil/ptrproxy.h>
#include <orcaiceutil/ptrbuffer.h>

#include <orca/platform2d.h>
#include <orca/power.h>

// forward declarations
class DualCANIO;
class rmpusb_frame_t;
class CanPacket;

class SegwayRmpUsb : public RmpDriverContext, public orcaiceutil::Thread
{
public:

    SegwayRmpUsb( orcaiceutil::PtrBuffer* position2dBuf, orcaiceutil::PtrBuffer* powerBuf,
                  orcaiceutil::PtrProxy* position, orcaiceutil::PtrProxy* commands, orcaiceutil::PtrProxy* power );
    virtual ~SegwayRmpUsb();

    void setup( const Ice::PropertiesPtr & );
    void configure( );

    virtual void activate();
    virtual void deactivate();

    virtual void repair() {};
    virtual void cancel() {};

    virtual void run();

private:

    // component/driver interface
    orcaiceutil::PtrBuffer* position2dBuf_;
    orcaiceutil::PtrBuffer* powerBuf_;
    orcaiceutil::PtrProxy* position2dProxy_;
    orcaiceutil::PtrProxy* commandProxy_;
    orcaiceutil::PtrProxy* powerProxy_;

    // internal storage
    orca::Position2dDataPtr position2dData_;
    orca::Velocity2dCommandPtr commandData_;
    orca::PowerDataPtr powerData_;

    // driver/hardware interface
    DualCANIO *canio_;
    rmpusb_frame_t* data_frame_;

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

    // helper to read a cycle of data from the RMP
    int read();

    void updateData( rmpusb_frame_t * );

    // helper to take a player command and turn it into a CAN command packet
    void makeVelocityCommand( CanPacket* pkt );

    // Calculate the difference between two raw counter values, taking care
    // of rollover.
    int diff(uint32_t from, uint32_t to, bool first);

    // helper to write a packet
    int write(CanPacket& pkt);

    // chip's utilities
    void WatchPacket( CanPacket* pkt, short int pktID );
    void WatchDataStream( CanPacket& pkt );

    bool firstread_;

};

#endif
