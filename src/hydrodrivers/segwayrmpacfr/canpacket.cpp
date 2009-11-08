/*
 *Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <stdio.h>
#include "rmpdefs.h"
#include "canpacket.h"
#include <stdlib.h>
#include <sstream>
#include <gbxutilacfr/exceptions.h>

using namespace std;

// Copied this from <canlib.h>. 
// This definition appears to be standard only for the FTDI USB to can
// The peak driver uses a different value in the MSG_Type field to denote
// a standard not extended message
#ifndef canMSG_STD
  #define canMSG_STD              0x0002
#endif

namespace segwayrmpacfr {

/*
    This code originates from Player/Stage project
    http://playerstage.sf.net
    file: canio.h
*/
CanPacket::CanPacket( uint32_t id )
    : id_(id)
{
    memset(msg_,0,sizeof(msg_));

//    flags = canMSG_STD;
//    dlc = 8;                 //Data Length Code
}

uint16_t CanPacket::getSlot(int s)  const
{
    return (uint16_t) ((msg_[s*2] << 8) | (msg_[s*2+1]));
}

void CanPacket::putSlot(const int slot, const uint16_t val)
{
    msg_[slot*2] = (val >> 8) & 0xFF;
    msg_[slot*2+1] = val & 0xFF;
}

char* CanPacket::toString() const
{
    static char buf[256];
    sprintf(buf, "id_:%04lX %02X %02X %02X %02X %02X %02X %02X %02X",
            id_, msg_[0], msg_[1], msg_[2], msg_[3], msg_[4], msg_[5],
            msg_[6], msg_[7]);

    return buf;
}

//////////////////////////////////////////////////////////////////////

void
checkSpeedLimits( int16_t speedCount )
{
    if ( abs(speedCount) > RMP_MAX_TRANS_VEL_COUNT )
    {
        stringstream ss;
        ss << "Asked for speed="<<speedCount<<"cnts, max="<<RMP_MAX_TRANS_VEL_COUNT<<"cnts";
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }
}

void
checkTurnrateLimits( int16_t turnrateCount )
{
    if ( abs(turnrateCount) > RMP_MAX_ROT_VEL_COUNT )
    {
        stringstream ss;
        ss << "Asked for turnrate="<<turnrateCount<<"cnts, max="<<RMP_MAX_ROT_VEL_COUNT<<"cnts";
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }    
}

CanPacket
statusCommandPacket( uint16_t statusCommandType, 
                     uint16_t value,
                     int16_t  speedCount,
                     int16_t  turnrateCount )
{
    checkSpeedLimits( speedCount );
    checkTurnrateLimits( turnrateCount );

    CanPacket pkt( RMP_CAN_ID_COMMAND );

    pkt.putSlot( 0, speedCount );
    pkt.putSlot( 1, turnrateCount );
    pkt.putSlot( 2, statusCommandType );
    pkt.putSlot( 3, value );

    return pkt;
}

CanPacket
motionCommandPacket( int16_t speedCount,
                     int16_t turnrateCount )
{
    checkSpeedLimits( speedCount );
    checkTurnrateLimits( turnrateCount );

    CanPacket pkt( RMP_CAN_ID_COMMAND );
    
    pkt.putSlot( 0, speedCount );
    pkt.putSlot( 1, turnrateCount );

    // Don't modify configuration
    pkt.putSlot( 2, 0 );

    return pkt;
}

CanPacket
shutdownCommandPacket()
{
    return CanPacket( RMP_CAN_ID_SHUTDOWN );
}

}
