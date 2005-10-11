/*
 *  Player - One Hell of a Robot Server
 *  Copyright (C) 2003  John Sweeney & Brian Gerkey
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */


#include "canio.h"
//#include "canio_kvaser.h"


#define RMP_CAN_ID_SHUTDOWN	0x0412
#define RMP_CAN_ID_COMMAND	0x0413
// 0x0400 = 1024
#define RMP_CAN_ID_MSG1		0x0400
#define RMP_CAN_ID_MSG2		0x0401
#define RMP_CAN_ID_MSG3		0x0402
#define RMP_CAN_ID_MSG4		0x0403
#define RMP_CAN_ID_MSG5		0x0404
#define RMP_CAN_ID_MSG6     0x0405
#define RMP_CAN_ID_MSG7     0x0406
#define RMP_CAN_ID_MSG8     0x0407

#define RMP_CAN_CMD_NONE		0
#define RMP_CAN_CMD_MAX_VEL		10
#define RMP_CAN_CMD_MAX_ACCL		11
#define RMP_CAN_CMD_MAX_TURN		12
#define RMP_CAN_CMD_GAIN_SCHED		13
#define RMP_CAN_CMD_CURR_LIMIT		14
#define RMP_CAN_CMD_RST_INT		50

#define RMP_CAN_RST_RIGHT		0x01
#define RMP_CAN_RST_LEFT		0x02
#define RMP_CAN_RST_YAW			0x04
#define RMP_CAN_RST_FOREAFT		0x08
#define RMP_CAN_RST_ALL			(RMP_CAN_RST_RIGHT | \
					 RMP_CAN_RST_LEFT | \
					 RMP_CAN_RST_YAW | \
					 RMP_CAN_RST_FOREAFT)

#define RMP_COUNT_PER_M			33215
#define RMP_COUNT_PER_DEG		7.8
#define RMP_COUNT_PER_M_PER_S		332
#define RMP_COUNT_PER_DEG_PER_S	7.8
#define RMP_COUNT_PER_MM_PER_S	0.32882963
#define RMP_COUNT_PER_DEG_PER_SS	7.8
#define RMP_COUNT_PER_REV          112644
#define RMP_COUNT_PER_VOLT         4.0
#define RMP_COUNT_PER_NM           1094
#define RMP_SEC_PER_FRAME          0.01

#define RMP_MAX_TRANS_VEL_MM_S		3576
#define RMP_MAX_ROT_VEL_DEG_S		18	// from rmi_demo: 1300*0.013805056
#define RMP_MAX_TRANS_VEL_COUNT		1176
#define RMP_MAX_ROT_VEL_COUNT		1024

#define RMP_GEOM_WHEEL_SEP 0.54

#define RMP_CAN_DROPPED_PACKET 0x5A5A

// this holds all the RMP data it gives us
class rmpusb_frame_t
{
  public:
    int16_t    pitch;
    int16_t    pitch_dot;
    int16_t    roll;
    int16_t    roll_dot;
    uint32_t   yaw;
    int16_t    yaw_dot;
    uint32_t   left;
    int16_t    left_dot;
    uint32_t   right;
    int16_t    right_dot;
    uint32_t   foreaft;
    int16_t    left_torque;
    int16_t    right_torque;

    uint16_t   frames;
    uint16_t   battery;

    //! 0=disabled, 1=tractor, 2=balance
    uint16_t controller_mode;
    //! 0=normal, 1=tall, 2=heavy
    uint16_t controller_gain_schedule;
    //! ???
    int16_t operational_state;
    //! Velocity command (as received)
    int16_t velocity_command;
    //! Rate command (as received)
    int16_t rate_command;

    // Is this frame ready (i.e., did the next frame start?)
    bool  is_ready;

    rmpusb_frame_t() { reset(); };

    // Adds a new packet to this frame
    void AddPacket(const CanPacket &pkt);

    int yes_[8];

    void dump()
    {
          printf("\n\nFRAME DATA:\n");
          pitch     != RMP_CAN_DROPPED_PACKET ? printf("pitch\t\t= %10i (%8X): %10.1f deg\n", (int)pitch, pitch, (float)pitch/RMP_COUNT_PER_DEG) : printf("pitch\t\t=    DROPPED\n");
          pitch_dot != RMP_CAN_DROPPED_PACKET ? printf("pitch_dot\t= %10i (%8X): %10.1f deg/s\n", (int)pitch_dot, pitch_dot, (float)pitch_dot/RMP_COUNT_PER_DEG) : printf("pitch_dot\t=    DROPPED\n");
          roll      != RMP_CAN_DROPPED_PACKET ? printf("roll\t\t= %10i (%8X): %10.1f deg\n", (int)roll, roll, (float)roll/RMP_COUNT_PER_DEG) : printf("roll\t\t=    DROPPED\n");
          roll_dot  != RMP_CAN_DROPPED_PACKET ? printf("roll_dot\t= %10i (%8X): %10.2f deg/s\n", (int)roll_dot, roll_dot, (float)roll_dot/RMP_COUNT_PER_DEG) : printf("roll_dot\t=    DROPPED\n");
          yaw       != RMP_CAN_DROPPED_PACKET ? printf("yaw\t\t= %10i (%8X): %10.2f deg\n", (int)yaw, yaw, (float)yaw/RMP_COUNT_PER_DEG) : printf("yaw\t\t=    DROPPED\n");
          yaw_dot   != RMP_CAN_DROPPED_PACKET ? printf("yaw_dot\t\t= %10i (%8X): %10.2f deg/s\n", (int)yaw_dot, yaw_dot, (float)yaw_dot/RMP_COUNT_PER_DEG) : printf("yaw_dot\t\t=    DROPPED\n");
          left      != RMP_CAN_DROPPED_PACKET ? printf("left\t\t= %10i (%8X): %10.2f m\n", (int)left, left, (float)left/RMP_COUNT_PER_M) : printf("left\t\t=    DROPPED\n");
          left_dot  != RMP_CAN_DROPPED_PACKET ? printf("left_dot\t= %10i (%8X): %10.2f m/s\n", (int)left_dot, left_dot, (float)left_dot/RMP_COUNT_PER_M) : printf("left_dot\t=    DROPPED\n");
          right     != RMP_CAN_DROPPED_PACKET ? printf("right\t\t= %10i (%8X): %10.2f m\n", (int)right, right, (float)right/RMP_COUNT_PER_M) : printf("right\t\t=    DROPPED\n");
          right_dot != RMP_CAN_DROPPED_PACKET ? printf("right_dot\t= %10i (%8X): %10.2f m/s\n", (int)right_dot, right_dot, (float)right_dot/RMP_COUNT_PER_M) : printf("right_dot\t=    DROPPED\n");
          foreaft   != RMP_CAN_DROPPED_PACKET ? printf("foreaft\t\t= %10i (%8X): %10.2f m\n", (int)foreaft, foreaft, (float)foreaft/RMP_COUNT_PER_M) : printf("foreaft\t\t=    DROPPED\n");
          left_torque    != RMP_CAN_DROPPED_PACKET ? printf("left_torque\t= %10i (%8X): %10.2f Nm\n", (int)left_torque, left_torque, (float)left_torque/RMP_COUNT_PER_NM) : printf("left_torque\t=    DROPPED\n");
          right_torque   != RMP_CAN_DROPPED_PACKET ? printf("right_torque\t= %10i (%8X): %10.2f Nm\n", (int)right_torque, right_torque, (float)right_torque/RMP_COUNT_PER_NM) : printf("right_torque\t=    DROPPED\n");
          frames    != RMP_CAN_DROPPED_PACKET ? printf("frames\t\t= %10i (%8X): %10i frame ctr\n", (int)frames, frames, (int)(frames)) : printf("frames\t\t=    DROPPED\n");
          battery   != RMP_CAN_DROPPED_PACKET ? printf("battery\t\t= %10i (%8X): %10.2f V\n", (int)battery, battery, (float)battery/RMP_COUNT_PER_VOLT) : printf("battery\t\t=    DROPPED\n");
    }

    void reset()
    {
        pitch                      = RMP_CAN_DROPPED_PACKET;
        pitch_dot                  = RMP_CAN_DROPPED_PACKET;
        roll                       = RMP_CAN_DROPPED_PACKET;
        roll_dot                   = RMP_CAN_DROPPED_PACKET;
        yaw                        = RMP_CAN_DROPPED_PACKET;
        yaw_dot                    = RMP_CAN_DROPPED_PACKET;
        left                       = RMP_CAN_DROPPED_PACKET;
        left_dot                   = RMP_CAN_DROPPED_PACKET;
        right                      = RMP_CAN_DROPPED_PACKET;
        right_dot                  = RMP_CAN_DROPPED_PACKET;
        foreaft                    = RMP_CAN_DROPPED_PACKET;
        left_torque                = RMP_CAN_DROPPED_PACKET;
        right_torque               = RMP_CAN_DROPPED_PACKET;
        frames                     = RMP_CAN_DROPPED_PACKET;
        battery                    = RMP_CAN_DROPPED_PACKET;
        controller_mode            = RMP_CAN_DROPPED_PACKET;
        controller_gain_schedule   = RMP_CAN_DROPPED_PACKET;
        operational_state          = RMP_CAN_DROPPED_PACKET;
        velocity_command           = RMP_CAN_DROPPED_PACKET;
        rate_command               = RMP_CAN_DROPPED_PACKET;

        is_ready = false;
        for ( int i=0; i<8; ++i ) yes_[i]=0;
    }
};



/* Takes a CAN packet from the RMP and parses it into a
 * rmpusb_frame_t struct.  sets the ready bitfield
 * depending on which CAN packet we have.  when
 * ready == 0x1F, then we have gotten 5 packets, so everything
 * is filled in.
 *
 * returns: 
 */
inline void
rmpusb_frame_t::AddPacket(const CanPacket &pkt)
{
    switch(pkt.id)
    {
    case RMP_CAN_ID_MSG1:
    {
        // no data, start of new frame
        // mark current frame finished, closed, or 'ready'
        is_ready = true;
        break;
    }
    case RMP_CAN_ID_MSG2:
    {
        pitch = pkt.GetSlot(0);
        pitch_dot = pkt.GetSlot(1);
        roll =  pkt.GetSlot(2);
        roll_dot =  pkt.GetSlot(3);
        yes_[1] = 1;
        break;
    }
    case RMP_CAN_ID_MSG3:
    {
        left_dot = (int16_t) pkt.GetSlot(0);
        right_dot = (int16_t) pkt.GetSlot(1);
        yaw_dot = (int16_t) pkt.GetSlot(2);
        frames = pkt.GetSlot(3);
        yes_[2] = 1;
        break;
    }
    case RMP_CAN_ID_MSG4:
    {
        left = (uint32_t)(((uint32_t)pkt.GetSlot(1) << 16) |
                    (uint32_t)pkt.GetSlot(0));
        right = (uint32_t)(((uint32_t)pkt.GetSlot(3) << 16) |
                (uint32_t)pkt.GetSlot(2));
        yes_[3] = 1;
        break;
    }
    case RMP_CAN_ID_MSG5:
    {
        foreaft = (uint32_t)(((uint32_t)pkt.GetSlot(1) << 16) |
                        (uint32_t)pkt.GetSlot(0));
        yaw = (uint32_t)(((uint32_t)pkt.GetSlot(3) << 16) |
                (uint32_t)pkt.GetSlot(2));
        yes_[4] = 1;
        break;
    }
    case RMP_CAN_ID_MSG6:
    {
        left_torque = (int16_t) pkt.GetSlot(0);
        right_torque = (int16_t) pkt.GetSlot(1);
        yes_[5] = 1;
        break;
    }
    case RMP_CAN_ID_MSG7:
    {
        controller_mode = (uint8_t) pkt.GetSlot(0);
        controller_gain_schedule = (uint8_t) pkt.GetSlot(1);
        operational_state = (int16_t) pkt.GetSlot(2);
        battery = pkt.GetSlot(3);
        yes_[6] = 1;
        break;
    }
    case RMP_CAN_ID_MSG8:
    {
        velocity_command = (int16_t) pkt.GetSlot(0);
        rate_command = (int16_t) pkt.GetSlot(1);
        yes_[7] = 1;
        break;
    }
    default:
    {
        // did not recognize the message
        break;
    }
  }

}
