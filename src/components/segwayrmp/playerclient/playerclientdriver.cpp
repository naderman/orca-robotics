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

#include <iostream>
#include <stdlib.h>
#include <assert.h>

//#include <orcaice/orcaice.h>
#include <orcaplayer/orcaplayer.h>
#include <libplayerc++/playerc++.h>
#include <IceUtil/Thread.h>     // for sleep()

#include "playerclientdriver.h"

using namespace std;
using namespace orca;
using namespace segwayrmp;
//using orcaice::operator<<;
using namespace PlayerCc;

// experiment to pass all custom props as a disctionary
//PlayerClientDriver::PlayerClientDriver( const std::map<std::string,std::string> & props, const std::string & prefix )

PlayerClientDriver::PlayerClientDriver( const char *host, int port )
    : enabled_( false ),
      robot_(0),
      positionProxy_(0),
      powerProxy_(0),
      host_(strdup(host)),
      port_(port)
{
}

PlayerClientDriver::~PlayerClientDriver()
{
}

int
PlayerClientDriver::enable()
{
    if ( enabled_ ) return 0;

    cout << "TRACE(playerclientdriver.cpp): PlayerClientDriver: Connecting to player on host "
         << host_ << ", port " << port_ << endl;
    
    // player throws exceptions on creation if we fail
    try
    {
        robot_      = new PlayerCc::PlayerClient( host_, port_ );
        positionProxy_ = new PlayerCc::Position2dProxy( robot_, 0 );
    
        robot_->Read();
    
        positionProxy_->SetMotorEnable( true );
    }
    catch ( const PlayerCc::PlayerError & e )
    {
        std::cerr << e << std::endl;
        cout << "ERROR(playerclientdriver.cpp): player error" << endl;
        disable();
        return -1;
    }

    enabled_ = true;
    return 0;
}

int PlayerClientDriver::repair()
{
    disable();
    return enable();
}

int
PlayerClientDriver::disable()
{
    if ( !enabled_ ) return 0;

    delete positionProxy_;
    delete powerProxy_;
    delete robot_;
    enabled_ = false;
    return 0;
}


int
PlayerClientDriver::read( orca::Position2dDataPtr &position2d, orca::PowerDataPtr &power,
                    std::string & status )
{
    if ( ! enabled_ ) {
        //cout << "ERROR(playerclientdriver.cpp): Can't read: not connected to Player/Stage yet." << endl;
        return -1;
    }

    // player throws exceptions on creation if we fail
    try
    {
        robot_->Read();
    }
    catch ( const PlayerCc::PlayerError & e )
    {
        std::cerr << e << std::endl;
        cout << "ERROR(playerclientdriver.cpp): Error reading from robot." << endl;
        enabled_ = false;
        return -1;
    }
    
    orcaplayer::convert( *positionProxy_, position2d );
    //orcaplayer::convert( *powerProxy_, power );

    status = "playing=1";

    return 0;
}

int
PlayerClientDriver::write( const orca::Velocity2dCommandPtr &position2d )
{
    // this version of Player client takes speed command in  [m, m, rad/s]
    try
    {
        positionProxy_->SetSpeed( position2d->motion.v.x, position2d->motion.v.y, position2d->motion.w );
    }
    catch ( const PlayerCc::PlayerError & e )
    {
        std::cerr << e << std::endl;
        cout << "ERROR(playerclientdriver.cpp): Error writing to robot." << endl;
        return -1;
    }
    return 0;     
}
