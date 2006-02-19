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

    robot_      = new PlayerClient( host_, port_ );
    positionProxy_ = new PositionProxy( robot_, 0 );

    // make sure we get read/write access ('a'=all)
    positionProxy_->ChangeAccess('a');
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

    if( positionProxy_->SetMotorState( 1 ) ) {
        cout << "ERROR(playerclientdriver.cpp): Error enabling motors" << endl;
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
    if ( ! enabled_ ) {
        //cout << "ERROR(playerclientdriver.cpp): Can't read: not connected to Player/Stage yet." << endl;
        return -1;
    }

    if ( robot_->Read() )
    {
        cout << "ERROR(playerclientdriver.cpp): Error reading from robot." << endl;
        return -1;
    }
    
    orcaplayer::convert( *positionProxy_, position2d );
    //orcaplayer::convert( *powerProxy_, power );

    return 0;
}

int
PlayerClientDriver::write( const orca::Velocity2dCommandPtr &position2d )
{
    // this version of Player client takes speed command in  [m, m, rad/s]
    // the return values are same as ours: 0 if ok, -1 if not.
    return positionProxy_->SetSpeed( position2d->motion.v.x, position2d->motion.v.y, position2d->motion.w );
}
