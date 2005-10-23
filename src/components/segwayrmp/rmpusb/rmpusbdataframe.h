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

// 0x0400 = 1024
#define RMP_CAN_ID_MSG1         0x0400
#define RMP_CAN_ID_MSG2         0x0401
#define RMP_CAN_ID_MSG3         0x0402
#define RMP_CAN_ID_MSG4         0x0403
#define RMP_CAN_ID_MSG5         0x0404
#define RMP_CAN_ID_MSG6         0x0405
#define RMP_CAN_ID_MSG7         0x0406
#define RMP_CAN_ID_MSG8         0x0407

#define RMP_CAN_ID_SHUTDOWN     0x0412
#define RMP_CAN_ID_COMMAND      0x0413
#define RMP_CAN_ID_HEARTBEAT    0x0688


// from Status Command table on page 7
#define RMP_CAN_CMD_NONE            0
#define RMP_CAN_CMD_MAX_VEL         10
#define RMP_CAN_CMD_MAX_ACCL        11
#define RMP_CAN_CMD_MAX_TURN        12
#define RMP_CAN_CMD_GAIN_SCHED      13
#define RMP_CAN_CMD_CURR_LIMIT      14
#define RMP_CAN_CMD_BAL_LOCKOUT     15
#define RMP_CAN_CMD_OPER_MODE       16
#define RMP_CAN_CMD_RST_INT         50

// from table: Bitfield for Reset Integrators on p.7
#define RMP_CAN_RST_RIGHT           0x01
#define RMP_CAN_RST_LEFT            0x02
#define RMP_CAN_RST_YAW             0x04
#define RMP_CAN_RST_FOREAFT         0x08
#define RMP_CAN_RST_ALL             (RMP_CAN_RST_RIGHT | \
                                     RMP_CAN_RST_LEFT | \
                                     RMP_CAN_RST_YAW | \
                                     RMP_CAN_RST_FOREAFT)

// unit conversions from Data Reference p.12

#define RMP_COUNT_PER_M            33215
#define RMP_COUNT_PER_M_PER_S      332

#define RMP_COUNT_PER_DEG          7.8
#define RMP_COUNT_PER_DEG_PER_S    7.8
#define RMP_COUNT_PER_DEG_PER_SS   7.8
#define RMP_COUNT_PER_REV          112644

// main (CU) battery voltage
#define RMP_CU_COUNT_PER_VOLT      4.0
// user interface battery voltage: volts=1.5V + n*counts
#define RMP_UI_COUNT_PER_VOLT      0.0125
// motor torque
#define RMP_COUNT_PER_NM           1094
#define RMP_SEC_PER_FRAME          0.01

// alexm: where is this from?
#define RMP_MAX_TRANS_VEL_MM_S      3576
// from rmi_demo: 1300*0.013805056
#define RMP_MAX_ROT_VEL_DEG_S       18

// limits

// from page 7 of the manual  
#define RMP_MAX_TRANS_VEL_COUNT     1176
#define RMP_MAX_ROT_VEL_COUNT       1024

// minimum CU battery voltage (V)
#define RMP_CU_MIN_VOLTAGE          66.0
// minimum UI battery voltage (V)
#define RMP_UI_MIN_VOLTAGE          6.0

// misc
#define RMP_GEOM_WHEEL_SEP          0.54

#define RMP_CAN_DROPPED_PACKET      0x5A5A

// need it for the CanPacket definition
#include "canpacket.h"

// this holds all the RMP data it gives us
class RmpUsbDataFrame
{
public:
    RmpUsbDataFrame();

    // Adds a new packet to this frame
    void AddPacket(const CanPacket &pkt);

    // print out frame data
    void dump();

    void reset();

    //! Pitch Angle
    int16_t    pitch;
    //! Pitch Rate
    int16_t    pitch_dot;
    //! Roll Angle
    int16_t    roll;
    //! Roll Rate
    int16_t    roll_dot;
    //! yaw displacement
    uint32_t   yaw;
    //! Yaw rate
    int16_t    yaw_dot;
    //! wheel displacement left
    uint32_t   left;
    //! Left wheel Velocity
    int16_t    left_dot;
    //! wheel displacement right
    uint32_t   right;
    //! Right wheel Velocity
    int16_t    right_dot;
    //! fore/aft displacement
    uint32_t   foreaft;
    //! Left Motor Torque
    int16_t    left_torque;
    //! Right Motor Torque
    int16_t    right_torque;
    //! Servo Frames
    uint16_t   frames;
    //! Open Circuit Battery Voltage (min of A, B)
    uint16_t   battery;
    //! Controller Mode (0= disabled, 1=tractor, 2= balance)
    uint16_t controller_mode;
    //! Controller Gain Schedule (0=normal, 1=tall, 2=heavy)
    uint16_t controller_gain_schedule;
    //! Operational State ???
    int16_t operational_state;
    //! Velocity command (as received)
    int16_t velocity_command;
    //! Turn Command (as received)
    int16_t rate_command;

    //! Is this frame ready (i.e., did the next frame start?)
    bool  isReady_;

    int msgCheckList_[8];
};


