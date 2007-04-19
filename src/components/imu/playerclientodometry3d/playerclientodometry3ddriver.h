/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_PLAYER_ODOMETRY3D_DRIVER_H
#define ORCA2_PLAYER_ODOMETRY3D_DRIVER_H

#include "../imudriver.h"
// #include "../sicklaserutil.h"

// Player proxies
namespace PlayerCc
{
class PlayerClient;
class Position3dProxy;
}

namespace imu {

/*
    @brief Interface to PlayerLaser laser.  Uses Player.

    @author Ben Upcroft
 */
class PlayerClientOdometry3dDriver : public ImuDriver
{
public:

    // The host and port are for player's usage
    PlayerClientOdometry3dDriver( const char *host, int port, bool isStage );
    virtual ~PlayerClientOdometry3dDriver();

    virtual int reset(){return 0;};

    // returns: 0 = success, non-zero = failure
    virtual int enable();
    virtual int disable();

    virtual bool isEnabled() { return enabled_; };

    virtual int read();

    virtual int getData(orca::ImuData& data );
    virtual int getData(orca::Odometry3dData& data );
//     virtual int getGeometry( orca::Position3dGeometryPtr& geometry );
    // virtual int setConfig( const orca::Position3dConfigPtr &config );

private:

    bool enabled_;
    PlayerCc::PlayerClient *robot_;
    PlayerCc::Position3dProxy *position3dProxy_;

    // orcaice::PtrBuffer<orca::Position3dGeometryPtr> position3dGeometryBuffer_;
    
    char *host_;
    int   port_;

    bool isStage_;
};

} // namespace

#endif
