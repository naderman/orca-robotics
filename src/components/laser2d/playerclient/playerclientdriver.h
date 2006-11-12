/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_PLAYER_LASER_DRIVER_H
#define ORCA2_PLAYER_LASER_DRIVER_H

#include "../driver.h"
#include "../sicklaserutil.h"

// Player proxies
namespace PlayerCc
{
class PlayerClient;
class LaserProxy;
}

namespace sicklaser {

/*
    @brief Interface to PlayerLaser laser.  Uses Player.

    @author Alex Brooks
 */
class PlayerClientDriver : public Driver
{
public:

    // The host and port are for player's usage
    PlayerClientDriver( const char *host, int port, int device, const std::string & playerDriver );
    virtual ~PlayerClientDriver();

    // returns: 0 = success, non-zero = failure
    virtual int enable();
    virtual int disable();

    virtual bool isEnabled() { return enabled_; };

    virtual int read( orca::LaserScanner2dDataPtr &data );

    virtual int getConfig( orca::RangeScanner2dConfigPtr &config );
    virtual int setConfig( const orca::RangeScanner2dConfigPtr &config );

private:

    bool enabled_;
    PlayerCc::PlayerClient *robot_;
    PlayerCc::LaserProxy *laserProxy_;

    char *host_;
    int   port_;
    int   device_;
    std::string playerDriver_;
};

}

#endif
