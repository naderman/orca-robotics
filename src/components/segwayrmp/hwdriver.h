/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_SEGWAY_RMP_HARDWARE_DRIVER_H
#define ORCA2_SEGWAY_RMP_HARDWARE_DRIVER_H

#include <orcarobotdriverutil/ihwdriver.h>
#include "types.h"

namespace segwayrmp
{

//
// Abstract interface class for something that talks to the RMP
// hardware (or a simulator).
//
// Member functions throw RmpExceptions on error conditions.
//
class HwDriver
{

public: 

    virtual ~HwDriver() {}

    // Throws std::exceptions on failure.
    // It is not an error to call this repeatedly: repeated calls should
    // re-initialise the hardware.
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

} // namespace

#endif
