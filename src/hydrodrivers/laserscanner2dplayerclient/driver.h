/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_LASERSCANNER2D_PLAYERCLIENT_H
#define HYDRO_LASERSCANNER2D_PLAYERCLIENT_H

#include <hydrointerfaces/laserscanner2d.h>

// Player proxies
namespace PlayerCc
{
class PlayerClient;
class LaserProxy;
}

namespace laserscanner2dplayerclient {

/*
    @brief Interface to PlayerLaser laser.  Uses Player.

    @author Alex Brooks
 */
class Driver : public hydrointerfaces::LaserScanner2d
{
public:

    Driver( const Config &cfg, const hydroutil::Context& context );
    virtual ~Driver();

    virtual void read( hydrointerfaces::LaserScanner2d::Data &data );

private:

    void init();

    Config config_;
    bool usePlayerTimestamp_;

    PlayerCc::PlayerClient *robot_;
    PlayerCc::LaserProxy *laser_;

    char *host_;
    int   port_;
    int   device_;
    std::string playerDriver_;

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
