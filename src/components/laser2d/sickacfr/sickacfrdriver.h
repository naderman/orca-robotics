/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCA2_LASER2D_SICK_ACFR_DRIVER_H
#define ORCA2_LASER2D_SICK_ACFR_DRIVER_H

#include <orcaice/context.h>
#include <driver.h>

// forward declaration of Carmen class
// class sick_laser_t;

namespace laser2d {

//
// This driver does not throw exceptions.
//
// @author Ben Upcroft
//
class SickAcfrDriver : public Driver
{

public: 

    SickAcfrDriver( const Config & cfg, const orcaice::Context & context );
    virtual ~SickAcfrDriver();

    // returns: 0 = success, non-zero = failure
    virtual int init();

    // Blocks till new data is available
    // returns: 0 = success, non-zero = failure
    virtual int read( orca::LaserScanner2dDataPtr &data );

    virtual const std::string heartbeatMessage();

private: 

   // int setupParams( double maxRange, int numberOfSamples, int baudrate );

    // core object
    // SickLaser* laser_;

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
