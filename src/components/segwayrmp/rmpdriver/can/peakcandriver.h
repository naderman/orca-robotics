
#ifndef PEAK_CAN_DRIVER_H_
#define PEAK_CAN_DRIVER_H_

#include <libpcan.h>
#include <rmpdriver/canpacket.h>
#include <rmpdriver/rmpio.h>
#include <orcaiceutil/timer.h>

namespace segwayrmp {

class RmpIo;
class RmpException;

//************************************************************
// A class to handle the Segway control using the peak
// CAN card....
// **NOTE That all function in the list may throw exceptions of 
// the type RmpException

// 
// Author Duncan Mercer
//  
 
class PeakCanDriver : public RmpIo{
    
public:
    // There should only be one instance of this class, No copying etc allowed!
    PeakCanDriver(const std::string & portName);
    PeakCanDriver(PeakCanDriver&){ assert(false); }; 

    ~PeakCanDriver();
    void enable(int debugLevel);
    void disable(void);
    RmpIo::RmpIoStatus readPacket(CanPacket *pkt);
    void writePacket(CanPacket *pkt);
      
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
    orcaiceutil::Timer writeTimer_;
    
}; // end of class declaration


}  // end of namespace

#endif
