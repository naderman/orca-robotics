/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Mathew Ridley, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_GPS_DRIVER_H
#define ORCA2_GPS_DRIVER_H

#include <orca/gps.h>
#include <string>

namespace gps {

static const ::std::string __orca__gps_default_heartbeat_msg = "";

/*

@brief Abstract class for a gps.

This guy is _not_ guaranteed to be thread-safe!

@author Matthew Ridley, Tobias Kaupp

*/
class Driver
{

public:

    Driver()
     : newGpsData_(false),
       newGpsMapGridData_(false),
       newGpsTime_(false),
       hasFix_(false)
    {};
       
    virtual ~Driver() {};

    // Initializes the device. If it's aleady initialized, then it
    // quietly re-initializes it.
    // returns: 0 = success, non-zero = failure
    virtual int init()=0;

    // Blocks till timout expires, returns number of messages read, -1 if failure
    virtual int read()=0;
    
    // Returns true if we have a GPS fix otherwise false
    virtual bool hasFix()=0;
    
    // Fetch latest GPS data. return -1 if unavailable
    virtual int getData(orca::GpsData& data )=0;
    
    // Fetch latest GPS Time information. return -1 if unavailable
    virtual int getTimeData(orca::GpsTimeData& data )=0;

    // mechanism to get error messages etc back from driver.
    virtual const std::string &infoMessages() { return infoMessages_; };

    // Any special info to put in the heartbeat messages?
    virtual const std::string heartbeatMessage() { return __orca__gps_default_heartbeat_msg; };

protected:
    // set to false by call to getData()
    bool newGpsData_;
    // set to false by call to getMapGridData()
    bool newGpsMapGridData_;
    // set to false by call to getTimeData()
    bool newGpsTime_;
    // do we have a fix yet ?
    bool hasFix_;

    orca::GpsData GpsData_;
    orca::GpsTimeData GpsTimeData_;

    std::string infoMessages_;

private:

};

}

#endif
