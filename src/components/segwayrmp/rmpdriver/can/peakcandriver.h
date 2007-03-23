//Note that this file must be included after rmpio.h in any project file

#ifndef PEAK_CAN_DRIVER_H_
#define PEAK_CAN_DRIVER_H_


namespace segwayrmp {

class RmpIo;

// A class to handle the Segway control to the peak
// CAN card....

class PeakCanDriver : public RmpIo{

    public:
        PeakCanDriver(const orcaice::Context & context );
        ~PeakCanDriver();
        void enable( int debugLevel );
        void disable( void );
        RmpIo::RmpIoStatus readPacket( CanPacket* pkt );
	void writePacket( CanPacket* pkt );
      
    private:
        HANDLE portHandle_;
        int debugLevel_;
	void convertCanPacketToPeak( TPCANMsg *peakCanOut, const CanPacket *pktIn );
	void convertPeakToCanPacket( CanPacket* pktOut, const TPCANMsg *peakCanIn );

};


} //end of namespace

#endif

