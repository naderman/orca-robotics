/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef SEGWAYRMP_RMPIO_H
#define SEGWAYRMP_RMPIO_H

namespace segwayrmpacfr
{

class CanPacket;

//
// @brief An abstract class for communicating with the RMP.
//        Note: This thing _has_ to be able to handle simultaneous read/write
//              from two different threads.
//
// @author Alex Brooks
//
class RmpIo
{

public: 

    enum RmpIoStatus
    {
        OK              = 0,
        NO_DATA         = 1,
    };

    virtual ~RmpIo() {}

    // Initiate comms with the RMP.
    virtual void enable( int debugLevel )=0;

    // Release all resources.
    virtual void disable()=0;
    
    // Returns OK if copied a packet, NO_DATA if not
    virtual RmpIoStatus readPacket( CanPacket &pkt )=0;    
    virtual void writePacket( const CanPacket &pkt )=0;

private: 


};

}

#endif
