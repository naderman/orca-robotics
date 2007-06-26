/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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
// @author Alex Brooks
//
class SickCarmenDriver : public Driver
{

public: 

    SickCarmenDriver( const Config & cfg, 
                      const orcaice::Context & context );
    virtual ~SickCarmenDriver();

    // Blocks till new data is available
    virtual void read( float *ranges, unsigned char *intensities, orca::Time &timeStamp );

private: 

    void init();

    int setupParams( double maxRange, int numberOfSamples, int baudrate );

    laser2d::Driver::Config config_;

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
