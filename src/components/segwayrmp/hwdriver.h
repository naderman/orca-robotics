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

    virtual ~HwDriver() {};
    
    virtual void enable()=0;
    virtual void disable()=0;
    // Blocking read.
    // Returns: 
    //   true:  important change in internal state occured (details can be read with getStatus).
    //   false: no important change
    virtual bool read( Data& data )=0;
    // Writes velocity command.
    virtual void write( const Command& command )=0;
    // Apply physical limits of this hardare.
    virtual void applyHardwareLimits( double& forwardSpeed, double& reverseSpeed, 
        double& turnrate, double& turnrateAtMaxSpeed ) {};
    // Non-blocking read of secondary data.
    virtual void get( Stats& stats ) {};
    // For debugging, convert to string as much of internal state as possible
    virtual std::string toString() { return std::string(""); };

    // Get information about the current status of the driver.
    // the string 'status' is a human-readable description.
    // Note that there are two ways for faults to be notified:
    //  - This function tells of hardware faults reported normally by the hardware.
    //  - Exceptions are thrown for non-recoverable faults, such as inability to
    //    communicate with the hardware.
    virtual void getStatus( std::string &status, bool &isWarn, bool &isFault )=0;
};

} // namespace

#endif
