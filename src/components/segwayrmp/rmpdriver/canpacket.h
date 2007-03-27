/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_SEGWAY_RMP_CAN_PACKET_H_
#define ORCA_SEGWAY_RMP_CAN_PACKET_H_

#include <sys/types.h>
#include <stdint.h>
#include <string.h>

namespace segwayrmp
{

/*
    @note This code originates from Player/Stage project
    http://playerstage.sf.net, file: canio.h
    @author Alex Makarenko
*/
class CanPacket
{
public:
    enum{ 
      CAN_DATA_SIZE=8
    }canInfo;
  
    CanPacket();
    
    uint16_t GetSlot(int s) const;
    char* toString();
    
    void PutSlot(const int slot, const uint16_t val);
    void PutByte(const int byte, const uint16_t val);
    
    long id;
    uint8_t msg[CAN_DATA_SIZE];
    uint32_t dlc;
    uint32_t flags;

};


} // namespace

#endif
