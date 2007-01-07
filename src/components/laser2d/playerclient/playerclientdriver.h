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
#include <driver.h>

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

    // The host and port are for player's usage
    PlayerClientDriver( const Config & cfg, const orcaice::Context & context );
    virtual ~PlayerClientDriver();

    // returns: 0 = success, non-zero = failure
    virtual int init();

    virtual int read( orca::LaserScanner2dDataPtr &data );

private:
    int setupParams( double maxRange, int numberOfSamples, int baudrate );

    PlayerCc::PlayerClient *robot_;
    PlayerCc::LaserProxy *laser_;

    char *host_;
    int   port_;
    int   device_;
    std::string playerDriver_;

    orcaice::Context context_;
};

} // namespace

#endif
