/*
 *  Orca-Components: Components for robotics.
 *  
 *  Copyright (C) 2004
 *  
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef ORCA_SEGWAY_RMP_CAN_PACKET_H_
#define ORCA_SEGWAY_RMP_CAN_PACKET_H_

#include <sys/types.h>
#include <stdint.h>
#include <string.h>


/*!
    @note This code originates from Player/Stage project
    http://playerstage.sf.net, file: canio.h
    @author Alex Makarenko
*/
class CanPacket
{
public:
    CanPacket();
    
    uint16_t GetSlot(int s) const;
    char* toString();
    
    void PutSlot(const int slot, const uint16_t val);
    void PutByte(const int byte, const uint16_t val);
    
    long id;
    uint8_t msg[8];
    uint32_t dlc;
    uint32_t flags;

};

#endif
