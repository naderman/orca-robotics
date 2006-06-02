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

#include "stagedriver.h"

using namespace std;
using namespace orca;
using namespace simlocaliser;
using namespace PlayerCc;

StageDriver::StageDriver( const char *host, int port, const char* id )
    : enabled_( false ),
      robot_(0),
      simulationProxy_(0),
      host_(strdup(host)),
      port_(port),
      id_(strdup(id))
{
}

StageDriver::~StageDriver()
{
}

int
StageDriver::enable()
{
    if ( enabled_ ) return 0;

    cout << "TRACE(playerclientdriver.cpp): StageDriver: Connecting to player on host "
         << host_ << ", port "<<port_ << ", id "<<id_<<endl;
    
    // player throws exceptions on creation if we fail
    try
    {
        robot_      = new PlayerCc::PlayerClient( host_, port_ );
        simulationProxy_ = new PlayerCc::SimulationProxy( robot_, 0 );
    
//         robot_->Read();
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

int StageDriver::repair()
{
    disable();
    return enable();
}

int
StageDriver::disable()
{
    if ( !enabled_ ) return 0;

    delete simulationProxy_;
    delete robot_;
    enabled_ = false;
    return 0;
}


int
StageDriver::read( orca::Localise2dDataPtr & localise2d )
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
    
     orcaplayer::convert( *simulationProxy_, localise2d, id_ );

    return 0;
}
