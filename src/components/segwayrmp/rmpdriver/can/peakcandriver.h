

#ifndef PEAK_CAN_DRIVER_H_
#define PEAK_CAN_DRIVER_H_


using namespace segwayrmp;
namespace orcacan {


// A class to handle the Segway control to the peak
// CAN card....

class PeakCanDriver : public HwDriver{


    public:
        PeakCanDriver(const orcaice::Context & context );
        ~PeakCanDriver();
        int enable(void) {return 0;};
        int disable(void) { return 0; }; //**??** should I do more here?
        int read( SegwayRmpData& data, std::string &status );
	int write( const SegwayRmpCommand& command ) {return 0;};
     
        void applyHardwareLimits( double& forwardSpeed, double& reverseSpeed, 
				  double& turnrate, double& turnrateAtMaxSpeed ){/**??** Does Nothing yet!*/};

        int get( SegwayRmpStats& stats ){return 0;};
        //std::string toString(){return 0;};
  
    private:
        HANDLE port_handle;

};


} //end of namespace

#endif

