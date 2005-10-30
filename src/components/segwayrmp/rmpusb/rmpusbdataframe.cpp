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

#include <stdio.h>

#include "rmpusbdataframe.h"

// this holds all the RMP data it gives us
RmpUsbDataFrame::RmpUsbDataFrame()
{
    reset();
}

void RmpUsbDataFrame::dump()
{
    printf("\n\nFRAME DATA:\n");
    msgCheckList_[1] ? printf("pitch\t\t= %10i (%8X): %10.1f deg\n", (int)pitch, pitch, (float)pitch/RMP_COUNT_PER_DEG) : printf("pitch\t\t=    DROPPED\n");
    msgCheckList_[1] ? printf("pitch_dot\t= %10i (%8X): %10.1f deg/s\n", (int)pitch_dot, pitch_dot, (float)pitch_dot/RMP_COUNT_PER_DEG) : printf("pitch_dot\t=    DROPPED\n");
    msgCheckList_[1] ? printf("roll\t\t= %10i (%8X): %10.1f deg\n", (int)roll, roll, (float)roll/RMP_COUNT_PER_DEG) : printf("roll\t\t=    DROPPED\n");
    msgCheckList_[1] ? printf("roll_dot\t= %10i (%8X): %10.2f deg/s\n", (int)roll_dot, roll_dot, (float)roll_dot/RMP_COUNT_PER_DEG) : printf("roll_dot\t=    DROPPED\n");
    msgCheckList_[4] ? printf("yaw\t\t= %10i (%8X): %10.2f deg\n", (int)yaw, yaw, (float)yaw/RMP_COUNT_PER_DEG) : printf("yaw\t\t=    DROPPED\n");
    msgCheckList_[2] ? printf("yaw_dot\t\t= %10i (%8X): %10.2f deg/s\n", (int)yaw_dot, yaw_dot, (float)yaw_dot/RMP_COUNT_PER_DEG) : printf("yaw_dot\t\t=    DROPPED\n");
    msgCheckList_[3] ? printf("left\t\t= %10i (%8X): %10.2f m\n", (int)left, left, (float)left/RMP_COUNT_PER_M) : printf("left\t\t=    DROPPED\n");
    msgCheckList_[2] ? printf("left_dot\t= %10i (%8X): %10.2f m/s\n", (int)left_dot, left_dot, (float)left_dot/RMP_COUNT_PER_M) : printf("left_dot\t=    DROPPED\n");
    msgCheckList_[3] ? printf("right\t\t= %10i (%8X): %10.2f m\n", (int)right, right, (float)right/RMP_COUNT_PER_M) : printf("right\t\t=    DROPPED\n");
    msgCheckList_[2] ? printf("right_dot\t= %10i (%8X): %10.2f m/s\n", (int)right_dot, right_dot, (float)right_dot/RMP_COUNT_PER_M) : printf("right_dot\t=    DROPPED\n");
    msgCheckList_[4] ? printf("foreaft\t\t= %10i (%8X): %10.2f m\n", (int)foreaft, foreaft, (float)foreaft/RMP_COUNT_PER_M) : printf("foreaft\t\t=    DROPPED\n");
    msgCheckList_[5] ? printf("left_torque\t= %10i (%8X): %10.2f Nm\n", (int)left_torque, left_torque, (float)left_torque/RMP_COUNT_PER_NM) : printf("left_torque\t=    DROPPED\n");
    msgCheckList_[5] ? printf("right_torque\t= %10i (%8X): %10.2f Nm\n", (int)right_torque, right_torque, (float)right_torque/RMP_COUNT_PER_NM) : printf("right_torque\t=    DROPPED\n");
    msgCheckList_[2] ? printf("frames\t\t= %10i (%8X): %10i frame ctr\n", (int)frames, frames, (int)(frames)) : printf("frames\t\t=    DROPPED\n");
    msgCheckList_[6] ? printf("battery\t\t= %10i (%8X): %10.2f V\n", (int)ui_battery, ui_battery, 1.5+(float)ui_battery*RMP_UI_COEFF) : printf("battery\t\t=    DROPPED\n");
    msgCheckList_[6] ? printf("battery\t\t= %10i (%8X): %10.2f V\n", (int)base_battery, base_battery, (float)base_battery/RMP_BASE_COUNT_PER_VOLT) : printf("battery\t\t=    DROPPED\n");
}

void RmpUsbDataFrame::reopen()
{
    msgCheckList_[0] = false;
}

void RmpUsbDataFrame::reset()
{
    for ( int i=0; i<8; ++i ) {
        msgCheckList_[i] = false;
    }
}

bool RmpUsbDataFrame::isClosed() const
{
    return msgCheckList_[0];
}

bool RmpUsbDataFrame::isComplete() const
{
    bool complete = true;
    for ( int i=0; i<8; ++i ) {
        complete = (complete && msgCheckList_[i]);
    }
    return complete;
}

/*!
 * Takes a CAN packet from the RMP and parses it into a
 * RmpUsbDataFrame struct.  sets the ready bitfield
 * depending on which CAN packet we have.  when
 * ready == 0x1F, then we have gotten 5 packets, so everything
 * is filled in.
 *
 */
void RmpUsbDataFrame::AddPacket(const CanPacket* pkt)
{
    switch(pkt->id)
    {
    case RMP_CAN_ID_MSG0:
    {
        // slot0 undefined
        // slot1 undefined
        // slot2 undefined
        // slot3 undefined

        // mark current frame finished, closed, or 'ready'
        msgCheckList_[0] = true;
        break;
    }
    case RMP_CAN_ID_MSG1:
    {
        pitch = pkt->GetSlot(0);
        pitch_dot = pkt->GetSlot(1);
        roll =  pkt->GetSlot(2);
        roll_dot =  pkt->GetSlot(3);

        msgCheckList_[1] = true;
        break;
    }
    case RMP_CAN_ID_MSG2:
    {
        left_dot = (int16_t) pkt->GetSlot(0);
        right_dot = (int16_t) pkt->GetSlot(1);
        yaw_dot = (int16_t) pkt->GetSlot(2);
        frames = pkt->GetSlot(3);

        msgCheckList_[2] = true;
        break;
    }
    case RMP_CAN_ID_MSG3:
    {
        left = (uint32_t)(((uint32_t)pkt->GetSlot(1) << 16) |
                    (uint32_t)pkt->GetSlot(0));
        right = (uint32_t)(((uint32_t)pkt->GetSlot(3) << 16) |
                (uint32_t)pkt->GetSlot(2));

        msgCheckList_[3] = true;
        break;
    }
    case RMP_CAN_ID_MSG4:
    {
        foreaft = (uint32_t)(((uint32_t)pkt->GetSlot(1) << 16) |
                        (uint32_t)pkt->GetSlot(0));
        yaw = (uint32_t)(((uint32_t)pkt->GetSlot(3) << 16) |
                (uint32_t)pkt->GetSlot(2));

        msgCheckList_[4] = true;
        break;
    }
    case RMP_CAN_ID_MSG5:
    {
        left_torque = (int16_t) pkt->GetSlot(0);
        right_torque = (int16_t) pkt->GetSlot(1);
        // slot2 undefined
        // slot3 undefined

        msgCheckList_[5] = true;
        break;
    }
    case RMP_CAN_ID_MSG6:
    {
        operational_mode = (uint8_t) pkt->GetSlot(0);
        controller_gain_schedule = (uint8_t) pkt->GetSlot(1);
        ui_battery = (int16_t) pkt->GetSlot(2);
        base_battery = (int16_t)pkt->GetSlot(3);

        msgCheckList_[6] = true;
        break;
    }
    case RMP_CAN_ID_MSG7:
    {
        velocity_command = (int16_t) pkt->GetSlot(0);
        rate_command = (int16_t) pkt->GetSlot(1);
        // slot2 undefined
        // slot3 undefined

        msgCheckList_[7] = true;
        break;
    }
    default:
    {
        // did not recognize the message
        break;
    }
  }

}
