#ifndef ORCAROBOTDRIVERUTIL_IHWDRIVER_H
#define ORCAROBOTDRIVERUTIL_IHWDRIVER_H

#include <string>
#include <IceUtil/IceUtil.h>

namespace orcarobotdriverutil {

//!
//! @brief Abstract driver class which can be managed by HwDriverHandler
//!
//! @author Alex Brooks
//!
template<typename Command, typename Data>
class IHwDriver
{

public: 

    virtual ~IHwDriver() {}

    // Throws std::exceptions on failure.
    // It is not an error to call this repeatedly: repeated calls should
    // re-initialise the hardware
    // (eg this will be called if the hardware reports an error condition)
    virtual void enable()=0;

    // Blocking read.
    // Returns: 
    //   true:  important change in internal state occured (details can be read with getStatus).
    //   false: no important change
    virtual bool read( Data &data )=0;
    
    // Writes command.
    virtual void write( const Command& command )=0;

    // Get information about the current status of the driver.
    // the string 'status' is a human-readable description.
    // Note that there are two ways for faults to be notified:
    //  - This function tells of hardware faults reported normally by the hardware.
    //  - Exceptions can be thrown from read/write for non-recoverable faults, such as inability to
    //    communicate with the hardware.
    virtual void getStatus( std::string &status, bool &isWarn, bool &isFault )=0;

private: 

};

}

#endif
