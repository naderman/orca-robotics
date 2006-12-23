/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#ifndef ORCA2_GPS_DRIVER_H
#define ORCA2_GPS_DRIVER_H

#include <orca/gps.h>
#include <string>


static const ::std::string __orca__gps_default_heartbeat_msg = "";

/*

@brief Abstract class for a gps.

All gpsi should implement this, making it simple
to add support for different gpsi.

This guy is _not_ guaranteed to be thread-safe!

@author Matthew Ridley

*/
class GpsDriver
{

public:

    GpsDriver() {};
    virtual ~GpsDriver() {};

    virtual int reset()=0;

    virtual int enable()=0;
    virtual int disable()=0;

    virtual bool isEnabled()=0;

    virtual bool hasFix()=0;

    // Blocks till timout expires, returns number of messages read, -1 if failure
    virtual int read()=0;
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

#endif
