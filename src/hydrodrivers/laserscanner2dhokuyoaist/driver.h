/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Michael Moser
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_LASERSCANNER2D_HOKUYO_AIST_H
#define HYDRO_LASERSCANNER2D_HOKUYO_AIST_H

#include <hydrointerfaces/laserscanner2d.h>
#include <memory>

namespace hokuyo_aist {
    class HokuyoLaser;
}

namespace laserscanner2dhokuyoaist {

class Driver : public hydrointerfaces::LaserScanner2d
{

public: 

    Driver( const Config & cfg, const hydroutil::Context & context );
    ~Driver();

    // Blocks till new data is available
    virtual void read( hydrointerfaces::LaserScanner2d::Data &data );

private: 
    void init();

    std::auto_ptr<hokuyo_aist::HokuyoLaser> laser_;

    Config config_;
    hydroutil::Context context_;
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
