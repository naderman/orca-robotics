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

#include "canpacket.h"

// Copied this from <canlib.h>. I assume that it's portable across CAN
// implementations.
#ifndef canMSG_STD
  #define canMSG_STD              0x0002
#endif


/*
    This code originates from Player/Stage project
    http://playerstage.sf.net
    file: canio.h
*/
CanPacket::CanPacket()
{
    memset(msg,0,sizeof(msg));

    flags = canMSG_STD;
    dlc = 8;
}

uint16_t CanPacket::GetSlot(int s)  const
{
    return (uint16_t) ((msg[s*2] << 8) | (msg[s*2+1]));
}

void CanPacket::PutSlot(const int slot, const uint16_t val)
{
    msg[slot*2] = (val >> 8) & 0xFF;
    msg[slot*2+1] = val & 0xFF;
}

void CanPacket::PutByte(const int byte, const uint16_t val)
{
    msg[byte] = val & 0xFF;
}

char* CanPacket::toString()
{
    static char buf[256];
    sprintf(buf, "id:%04lX %02X %02X %02X %02X %02X %02X %02X %02X",
            id, msg[0], msg[1], msg[2], msg[3], msg[4], msg[5],
            msg[6], msg[7]);

    return buf;
}
