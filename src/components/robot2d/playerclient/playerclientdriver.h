/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ROBOT_PLAYER_CLIENT_DRIVER_H
#define ORCA2_ROBOT_PLAYER_CLIENT_DRIVER_H

#include "../hwdriver.h"
#include "playerclientdriverconfig.h"

// Player proxies
namespace PlayerCc
{
class PlayerClient;
class Position2dProxy;
}

namespace robot2d
{

class PlayerClientDriver : public HwDriver
{
public:

    PlayerClientDriver( const orcaice::Context & context );
    //PlayerClientDriver( const std::map<std::string,std::string> & props );
    virtual ~PlayerClientDriver();

    // returns: 0 = success, non-zero = failure
    virtual int enable();
    virtual int repair();
    virtual int disable();

    virtual int read( Data& data, std::string & status );

    virtual int write( const Command& command );

private:

    bool enabled_;
    PlayerCc::PlayerClient *robot_;
    PlayerCc::Position2dProxy *positionProxy_;

    // configuration
    orcaice::Context context_;
    PlayerClientDriverConfig config_;
};

} // namespace

#endif
