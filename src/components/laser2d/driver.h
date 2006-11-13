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

    struct Config
    {
        double maxRange;
        double fieldOfView;
        double startAngle;
        int    numberOfReturns;
    };

    Driver()
        : isEnabled_(false) {};
    virtual ~Driver() {};

    virtual int enable()=0;
    virtual int disable()=0;

    virtual bool isEnabled() { return isEnabled_; };

    // Blocks till new data is available
    virtual int read( orca::LaserScanner2dDataPtr &data )=0;

    // Get the current configuration
    virtual int getConfig( Config &cfg )=0;

    // Set a specifc configuration
    virtual int setConfig( const Config &cfg )=0;

    // mechanism to get error messages etc back from driver.
    virtual const std::string &infoMessages() { return infoMessages_; };

    // Any special info to put in the heartbeat messages?
    virtual const std::string heartbeatMessage() { return __orca__laserdriver_default_heartbeat_msg; };

protected:

    bool isEnabled_;

    Config currentConfig_;

    std::string infoMessages_;

private:

};

}

#endif
