#ifndef HYDRORMPUTIL_POWERBASEMANAGER_H
#define HYDRORMPUTIL_POWERBASEMANAGER_H

#include <hydrormputil/driverthread.h>
#include <memory>
#include <boost/shared_ptr.hpp>

namespace hydrormputil {

//
// @brief Aggregates information about multiple powerbases, to give
//        the impression of a single robot.
//
class AggregatorCallback {
public:
    virtual ~AggregatorCallback() {}
    
    virtual void hardwareInitialised( int powerbaseID )=0;
    virtual void receiveData( int                                     powerbaseID,
                              const hydrointerfaces::SegwayRmp::Data &data )=0;
    
};

//
// @brief Manages a single power-base (an RMP-style vehicle may be made up of multiple (e.g. the RMP400 has two)
//
// @author Alex Brooks
//
class PowerbaseManager : public hydrormputil::Callback
{
public: 

    PowerbaseManager( const std::string                         &name,
                      int                                        powerbaseID,
                      AggregatorCallback                        &aggregatorCallback,
                      std::auto_ptr<hydrointerfaces::SegwayRmp> &hydroDriver,
                      const hydroutil::Context                  &context );
    ~PowerbaseManager();

    hydrointerfaces::SegwayRmp::Capabilities capabilities() const
        { return hydroDriver_->capabilities(); }

    void init( const hydrormputil::DriverThread::Config &cfg );
    void startThread();

    const std::string &name() const { return name_; }

    void setDesiredSpeed( const hydrointerfaces::SegwayRmp::Command &cmd );

    // from hydrormputil::Callback
    void hardwareInitialised()
        { aggregatorCallback_.hardwareInitialised( powerbaseID_ ); }
    void receiveData( const hydrointerfaces::SegwayRmp::Data &data,
                      hydrormputil::StallType                  stallType )
        { 
            stallType_ = stallType;
            aggregatorCallback_.receiveData( powerbaseID_, data ); 
        }

    hydrormputil::StallType stallType() const { return stallType_; }

private: 

    hydrormputil::DriverThread *segwayRmpDriverThread_;
    gbxiceutilacfr::ThreadPtr segwayRmpDriverThreadPtr_;

    // Generic hydro driver for the hardware
    std::auto_ptr<hydrointerfaces::SegwayRmp> hydroDriver_;

    const std::string name_;
    const int         powerbaseID_;

    AggregatorCallback &aggregatorCallback_;

    hydrormputil::StallType stallType_;

    hydroutil::Context context_;
};
typedef boost::shared_ptr<PowerbaseManager> PowerbaseManagerPtr;

}

#endif
