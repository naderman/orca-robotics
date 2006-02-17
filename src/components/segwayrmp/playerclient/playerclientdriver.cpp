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

#include "playerclientdriver.h"

#include <iostream>
#include <stdlib.h>
#include <assert.h>

#include <playerclient.h>
//#include <orcaice/orcaice.h>
#include <orcaplayer/orcaplayer.h>
#include <IceUtil/Thread.h>     // for sleep()

using namespace std;
using namespace orca;
//using orcaice::operator<<;


//PlayerClientDriver::PlayerClientDriver( const std::map<std::string,std::string> & props )
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

    robot_      = new PlayerClient( host_, port_ );
    positionProxy_ = new PositionProxy( robot_, 0 );

    positionProxy_->ChangeAccess('r');
    if(positionProxy_->GetAccess() == 'e')
    {
        cout << "ERROR(playerclientdriver.cpp): couldn't get device access!" << endl;
        disable();
        return -1;
    }

    if ( robot_->Read() )
    {
        cout << "ERROR(playerclientdriver.cpp): Error reading from robot" << endl;
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
                    HwDriver::Status & status )
{
    if ( ! enabled_ )
    {
        cout << "ERROR(playerclientdriver.cpp): Can't read: not connected to Player/Stage yet. Sleeping for 1 sec..." << endl;
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
        return -1;
    }

    if ( robot_->Read() )
    {
        cout << "ERROR(playerclientdriver.cpp): Error reading from robot -- shutting down." << endl;
        enabled_ = false;
        return -1;
    }
    
    orcaplayer::convert( *positionProxy_, position2d );
    //orcaplayer::convert( *powerProxy_, power );

    return 0;
}

int
PlayerClientDriver::sendMotionCommand( orca::Velocity2dCommandPtr &position2d )
{
    return 0;
}
