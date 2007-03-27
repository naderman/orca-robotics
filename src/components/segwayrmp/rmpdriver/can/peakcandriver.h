
#ifndef PEAK_CAN_DRIVER_H_
#define PEAK_CAN_DRIVER_H_

#include <libpcan.h>
#include <rmpdriver/canpacket.h>
#include <rmpdriver/rmpio.h>


namespace segwayrmp {

class RmpIo;
class RmpException;

//************************************************************
// A class to handle the Segway control to the peak
// CAN card....
//**NOTE That all function in the list may throw exceptions of 
//the type RmpException
 
class PeakCanDriver : public RmpIo{

    public:
        PeakCanDriver(const std::string & portName);
        ~PeakCanDriver();
        void enable( int debugLevel );
        void disable( void );
        RmpIo::RmpIoStatus readPacket( CanPacket* pkt );
        void writePacket( CanPacket* pkt );
      
    private:
        HANDLE portHandle_;
        int debugLevel_;
        bool isEnabled_;
	    void convertCanPacketToPeak( TPCANMsg *peakCanOut, const CanPacket *pktIn );
	    void convertPeakToCanPacket( CanPacket* pktOut, const TPCANMsg *peakCanIn );
    
};


} //end of namespace

#endif
