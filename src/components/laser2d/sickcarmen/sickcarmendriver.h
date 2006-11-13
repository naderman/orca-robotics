/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCA2_LASER2D_SICK_CARMEN_DRIVER_H
#define ORCA2_LASER2D_SICK_CARMEN_DRIVER_H

#include <orcaice/context.h>
#include <driver.h>

// forward declaration of Carmen class
class sick_laser_t;

namespace laser2d {

//
// This driver does not throw exceptions.
//
// @author Alex Brooks
//
class SickCarmenDriver : public Driver
{

public: 

    SickCarmenDriver( const orcaice::Context & context );
    virtual ~SickCarmenDriver();

    // returns: 0 = success, non-zero = failure
    virtual int enable();
    virtual int disable();

    // Blocks till new data is available
    virtual int read( orca::LaserScanner2dDataPtr &data );

    virtual int getConfig( Config &cfg );
    virtual int setConfig( const Config &cfg );

    virtual const std::string heartbeatMessage();

private: 

    int setupParams( double maxRange, int numberOfReturns, int baudrate );

    // these versions don't clear infoMessage_ first.
    int doEnable();
    int doDisable();

    // carmen core object
    sick_laser_t *laser_;

    char *device_;
    // LMS or PMS
    char *type_; 
    int baudrate_;

    orcaice::Context context_;

    char sickInfoMessage_[2000];

    bool   firstRead_;
    double lastStatsUpdateTime_;
    bool   laserStalled_;
};

} // namespace

#endif
