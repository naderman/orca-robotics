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
#include <libplayerc++/playerc++.h>
#include <memory>
#include <hydroutil/store.h>

namespace robot2d
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

    hydroutil::Store<Command> commandStore_;

    std::auto_ptr<PlayerCc::PlayerClient>    robot_;
    std::auto_ptr<PlayerCc::Position2dProxy> positionProxy_;

    // configuration
    orcaice::Context context_;
    PlayerClientDriverConfig config_;
};

} // namespace

#endif
