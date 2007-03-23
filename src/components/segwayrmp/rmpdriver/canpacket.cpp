/*
 *Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <stdio.h>

#include "canpacket.h"

// Copied this from <canlib.h>. I assume that it's portable across CAN
// implementations.
#ifndef canMSG_STD
  #define canMSG_STD              0x0002
#endif

using namespace segwayrmp;

/*
    This code originates from Player/Stage project
    http://playerstage.sf.net
    file: canio.h
*/
CanPacket::CanPacket()
{
    memset(msg,0,sizeof(msg));

    flags = canMSG_STD;
    dlc = 8;                 //Data Length Code
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
