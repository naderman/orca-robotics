/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
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
  CAN Packets consist of:
    - Header (2 bytes in standard packet (which the RMP always uses))
    - Data Length Code (2 bytes, always '8' for RMP)
    - Data (8 bytes, split into 4 2-byte slots)

  Even if we're interfacing with USB, we still pass CAN packets
  (wrapped up in USB packets).

  @note This code originates from Player/Stage project
  http://playerstage.sf.net, file: canio.h
  @author Alex Makarenko
*/
class CanPacket
{
public:

    // Always use all 8 bytes
    static const int CAN_DATA_SIZE = 8;

    CanPacket( uint32_t id=0 );
    
    // Sets the ID (what the RMP docs call the header)
    void setId( uint32_t id ) { id_ = id; }

    // returns the value of the slotNum'th slot
    uint16_t getSlot(int slotNum) const;

    // sets the slotNum'th slot to val
    void putSlot(const int slotNum, const uint16_t val);
    
    // The id is the 'header' field in the RMP docs
    uint32_t id() const { return id_; }

    // Allow direct access for memcpy
    uint8_t *msg() { return msg_; }
    const uint8_t *msg() const { return msg_; }

    // Human-readable string for debugging
    char* toString();

private:

    long id_;
    uint8_t msg_[CAN_DATA_SIZE];

    // Could store dataLengthCode and flags here,
    // but don't bother cause for the RMP they're always the same.
    //    uint32_t dlc;
    //    uint32_t flags;

};

//
// Some functions for building CAN packets
// (all values are in the RMP's native representation)
//

CanPacket
statusCommandPacket( uint16_t statusCommandType, 
                     uint16_t value,
                     int16_t  speedCount,
                     int16_t  turnrateCount );

CanPacket
motionCommandPacket( int16_t speedCount,
                     int16_t turnrateCount );

CanPacket
shutdownCommandPacket();

} // namespace

#endif
