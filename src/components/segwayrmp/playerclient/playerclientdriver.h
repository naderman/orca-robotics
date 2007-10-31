/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_SEGWAY_RMP_PLAYER_CLIENT_DRIVER_H
#define ORCA2_SEGWAY_RMP_PLAYER_CLIENT_DRIVER_H

#include "../hwdriver.h"
#include "playerclientdriverconfig.h"
#include <IceUtil/IceUtil.h>

// Player proxies
namespace PlayerCc
{
class PlayerClient;
// class Position2dProxy;
class Position3dProxy;
class PowerProxy;
}

namespace segwayrmp
{

class PlayerClientDriver : public HwDriver
{
public:

    PlayerClientDriver( const orcaice::Context & context );
    virtual ~PlayerClientDriver();

    virtual void enable();

    virtual bool read( Data& data );

    virtual void write( const Command& command );

    virtual void getStatus( std::string &status, bool &isWarn, bool &isFault );

private:

    virtual void disable();

    bool enabled_;
    PlayerCc::PlayerClient *robot_;
//     PlayerCc::Position2dProxy *positionProxy_;
    PlayerCc::Position3dProxy *position3dProxy_;
    PlayerCc::PowerProxy *powerProxy_;

    // configuration
    orcaice::Context context_;
    PlayerClientDriverConfig config_;

    IceUtil::Mutex   mutex_;
};

} // namespace

#endif
