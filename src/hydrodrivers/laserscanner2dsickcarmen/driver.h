/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_LASERSCANNER2D_SICK_CARMEN_H
#define HYDRO_LASERSCANNER2D_SICK_CARMEN_H

#include <hydrointerfaces/laserscanner2d.h>

// forward declaration of Carmen class
class sick_laser_t;

namespace laserscanner2dsickcarmen {

//
// @author Alex Brooks
//
class Driver : public hydrointerfaces::LaserScanner2d
{

public: 

    Driver( const Config & cfg, 
                      const hydroutil::Context & context );
    virtual ~Driver();

    // Blocks till new data is available
    virtual void read( hydrointerfaces::LaserScanner2d::Data &data );

private: 

    void init();

    int setupParams( double maxRange, int numberOfSamples, int baudrate );

    Config config_;

    // carmen core object
    sick_laser_t *laser_;

    char *device_;
    // LMS or PMS
    char *type_; 
    int baudrate_;

    hydroutil::Context context_;

    char sickInfoMessage_[2000];

    bool   firstRead_;
    double lastStatsUpdateTime_;
    bool   laserStalled_;
};

// Used for dynamically loading driver
class Factory : public hydrointerfaces::LaserScanner2dFactory
{
public:
    hydrointerfaces::LaserScanner2d*
    createDriver( const hydrointerfaces::LaserScanner2d::Config &config,
                  const hydroutil::Context &context ) const
    {
        return new Driver( config, context );
    }
};

} // namespace

// Used for dynamically loading driver
extern "C" {
    hydrointerfaces::LaserScanner2dFactory *createDriverFactory();
}

#endif
