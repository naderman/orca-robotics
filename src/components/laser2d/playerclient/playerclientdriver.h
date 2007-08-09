/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_LASER2D_PLAYERCLIENT_DRIVER_H
#define ORCA2_LASER2D_PLAYERCLIENT_DRIVER_H

#include <orcaice/context.h>
#include <laser2dutil/driver.h>

// Player proxies
namespace PlayerCc
{
class PlayerClient;
class LaserProxy;
}

namespace laser2d {

/*
    @brief Interface to PlayerLaser laser.  Uses Player.

    @author Alex Brooks
 */
class PlayerClientDriver : public Driver
{
public:

    PlayerClientDriver( const Config &cfg, const orcaice::Context& context );
    virtual ~PlayerClientDriver();

    virtual void read( std::vector<float> &ranges, 
                       std::vector<unsigned char> &intensities, 
                       orca::Time &timeStamp );

private:

    void init();

    Driver::Config config_;

    PlayerCc::PlayerClient *robot_;
    PlayerCc::LaserProxy *laser_;

    char *host_;
    int   port_;
    int   device_;
    std::string playerDriver_;

    orcaice::Context context_;
};

// Used for dynamically loading driver
class PlayerClientDriverFactory : public laser2d::DriverFactory
{
public:
    laser2d::Driver *createDriver( const Driver::Config &config,
                                   const orcaice::Context &context ) const
        {
            return new PlayerClientDriver( config, context );
        }
};

} // namespace

// Used for dynamically loading driver
extern "C" {
    laser2d::DriverFactory *createDriverFactory();
}

#endif
