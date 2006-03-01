/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef ORCA2_ROBOT_PLAYER_CLIENT_DRIVER_H
#define ORCA2_ROBOT_PLAYER_CLIENT_DRIVER_H

#include "../hwdriver.h"

// Player proxies
namespace PlayerCc
{
class PlayerClient;
class Position2dProxy;
class PowerProxy;
}

class PlayerClientDriver : public HwDriver
{
public:

    PlayerClientDriver( const char *host, int port );
    //PlayerClientDriver( const std::map<std::string,std::string> & props );
    virtual ~PlayerClientDriver();

    // returns: 0 = success, non-zero = failure
    virtual int enable();
    virtual int repair();
    virtual int disable();

    virtual int read( orca::Position2dDataPtr &position2d, orca::PowerDataPtr &power,
                      std::string & status );

    virtual int write( const orca::Velocity2dCommandPtr &position2d );

private:

    bool enabled_;
    PlayerCc::PlayerClient *robot_;
    PlayerCc::Position2dProxy *positionProxy_;
    PlayerCc::PowerProxy *powerProxy_;
    
    char *host_;
    int   port_;
};

#endif
