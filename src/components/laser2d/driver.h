/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCA2_LASER2D_HW_DRIVER_H
#define ORCA2_LASER2D_HW_DRIVER_H

#include <string>
#include <orca/laserscanner2d.h>


static const ::std::string __orca__laserdriver_default_heartbeat_msg = "";

namespace laser2d {

/*
@brief Abstract class for a scanning laser.

All laser scanners should implement this, making it simple
to add support for different laser scanners.

This guy is _not_ guaranteed to be thread-safe!

@author Alex Brooks a.brooks at cas.edu.au
*/
class Driver 
{

public:

    class Config
    {   
    public:
        Config();
        bool validate() const;
        std::string toString() const;
        bool operator==( const Config & other );
        bool operator!=( const Config & other );

        double minRange;
        double maxRange;
        double fieldOfView;
        double startAngle;
        int    numberOfSamples;
    };

    Driver( const Config & cfg ) : config_(cfg) {};
    virtual ~Driver() {};

    // Initializes the device. If it's aleady initialized, then it
    // quietly re-initializes it.
    // returns: 0 = success, non-zero = failure
    virtual int init()=0;

    // Blocks till new data is available
    // returns: 0 = success, non-zero = failure
    virtual int read( orca::LaserScanner2dDataPtr &data )=0;

    // mechanism to get error messages etc back from driver.
    virtual const std::string &infoMessages() const { return infoMessages_; };

    // Any special info to put in the heartbeat messages?
    virtual const std::string heartbeatMessage()const { return __orca__laserdriver_default_heartbeat_msg; };

    // Simply returns the current stored configuration. Does not talk to device 
    Config config() const { return config_; };

protected:

    Config config_;

    std::string infoMessages_;
};

}

#endif
