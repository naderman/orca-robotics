/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 * Copyright (c) 2004-2009 Duncan Mercer
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA_CAN_PEAK_RMP_IO_H
#define ORCA_CAN_PEAK_RMP_IO_H

#include <libpcan.h>
#include "../canpacket.h"
#include "../rmpio.h"
#include <hydroutil/uncopyable.h>
#include <gbxsickacfr/gbxiceutilacfr/timer.h>

namespace segwayrmpacfr {

//************************************************************
// A class to handle the Segway control using the peak
// CAN card....
// **NOTE That all function in the list may throw exceptions of 
// the type hydrointerfaces::SegwayRmp::Exception

// 
// Author Duncan Mercer
//  
 
class CanPeakRmpIo : public RmpIo, public hydroutil::Uncopyable
{
    
public:
    // There should only be one instance of this class
    CanPeakRmpIo( const std::string & portName );
    ~CanPeakRmpIo();
    
    // from RmpIo
    virtual void enable(int debugLevel);
    virtual void disable(void);
    virtual RmpIo::RmpIoStatus readPacket( CanPacket &pkt );
    virtual void writePacket( const CanPacket &pkt );
      
private:
    HANDLE portHandle_;
    bool isEnabled_;
    int debugLevel_;

    // Private methods to swap between the data representation for the PCMCIA card and
    // that used by the rest of the segwayrmp driver code....
    void convertCanPacketToPeak(TPCANMsg *peakCanOut, const CanPacket *pktIn);
    void convertPeakToCanPacket(CanPacket *pktOut, const TPCANMsg *peakCanIn);

    // String representation of the peak can card error
    std::string peakStatusToString(DWORD status);
    gbxiceutilacfr::Timer writeTimer_;
    
}; // end of class declaration


}  // end of namespace

#endif
