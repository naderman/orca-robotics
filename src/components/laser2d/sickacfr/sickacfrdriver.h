/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCA2_LASER2D_SICK_ACFR_DRIVER_H
#define ORCA2_LASER2D_SICK_ACFR_DRIVER_H

#include <orcaice/context.h>
#include <laser2dutil/driver.h>

#include "laser.h"

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

    int setupParams( double maxRange, int numberOfSamples, int baudrate );

    // core object
    Laser* laser_;

    // struct for holding laser data and params
	LaserData rawLaserData_;

    char* device_;
    // LMS or PMS
    char* type_; 
    int baudrate_;

    char sickInfoMessage_[2000];

    // bool   firstRead_;
    // double lastStatsUpdateTime_;
    // bool   laserStalled_;

	int timeoutMs_;
    
    int laserCount_;

    orcaice::Context context_;
};

// Used for dynamically loading driver
class SickAcfrDriverFactory : public laser2d::DriverFactory
{
public:
    laser2d::Driver *createDriver( const Driver::Config &config,
                                   const orcaice::Context &context ) const
        {
            return new SickAcfrDriver( config, context );
        }
};

} // namespace

// Used for dynamically loading driver
extern "C" {
    laser2d::DriverFactory *createDriverFactory();
}

#endif
