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
    
// Exception class for GPS
class GpsException : public std::exception
{
public:

    GpsException(const char *message)
        : message_(message) {}
    GpsException(const std::string &message)
        : message_(message) {}

    virtual ~GpsException() throw() {}

    virtual const char* what() const throw() { return message_.c_str(); }

protected:

    std::string  message_;
};

/*

@brief Abstract class for a gps.

This guy is _not_ guaranteed to be thread-safe!

@author Matthew Ridley, Tobias Kaupp

*/
class Driver
{

public:

    Driver() {};
       
    virtual ~Driver() {};

    // Initializes the device. If it's aleady initialized, then it
    // quietly re-initializes it.
    // May throw GpsException
    virtual void init()=0;

    // Reads from GPS, blocks till timout expires
    // May throw GpsException
    virtual void read(orca::GpsData&)=0;
protected:

    orca::GpsData gpsData_;

};

}

#endif
