/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <stdlib.h>
#include <assert.h>

#include <orcaice/orcaice.h>
// #include <orcaplayer/orcaplayer.h>
#include <libplayerc++/playerc++.h>
#include <IceUtil/Thread.h>     // for sleep()

#include "playerclientdriver.h"

using namespace std;
using namespace orca;
using namespace robot2d;
using namespace PlayerCc;

// experiment to pass all custom props as a disctionary
//PlayerClientDriver::PlayerClientDriver( const std::map<std::string,std::string> & props, const std::string & prefix )

PlayerClientDriver::PlayerClientDriver( const orcaice::Context & context )
    : enabled_( false ),
      robot_(0),
      positionProxy_(0),
      context_(context)
{
    // parse configuration parameters
    readFromProperties( context, config_ );
    cout<<config_<<endl;
}

PlayerClientDriver::~PlayerClientDriver()
{
}

int
PlayerClientDriver::enable()
{
    if ( enabled_ ) return 0;

    cout << "TRACE(playerclientdriver.cpp): PlayerClientDriver: Connecting to player on host "
         << config_.host << ", port " << config_.port << endl;
    
    // player throws exceptions on creation if we fail
    try
    {
        robot_      = new PlayerCc::PlayerClient( config_.host, config_.port );
        positionProxy_ = new PlayerCc::Position2dProxy( robot_, 0 );
    
        robot_->Read();
    
        positionProxy_->SetMotorEnable( true );
    }
    catch ( const PlayerCc::PlayerError & e )
    {
        std::cerr << e << std::endl;
        cout << "ERROR(playerclientdriver.cpp): player error" << endl;
        disable();

        // Slow things down a little...
        sleep(1);
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
    delete robot_;
    enabled_ = false;
    return 0;
}


int
PlayerClientDriver::read( Data& data, std::string & status )
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
    
//     orcaplayer::convert( *positionProxy_, position2d );

    orca::Time t = orcaice::toOrcaTime( positionProxy_->GetDataTime() );
    data.seconds = t.seconds;
    data.useconds = t.useconds;

    data.x = positionProxy_->GetXPos();
    data.y = positionProxy_->GetYPos();
    data.o = positionProxy_->GetYaw();
    
    data.vx = positionProxy_->GetXSpeed();
    data.vy = positionProxy_->GetYSpeed();
    data.w = positionProxy_->GetYawSpeed();

    status = "playing=1";

    return 0;
}

int
PlayerClientDriver::write( const Command& command )
{
    // this version of Player client takes speed command in  [m, m, rad/s]
    try
    {
        positionProxy_->SetSpeed( command.vx, command.vy, command.w );
    }
    catch ( const PlayerCc::PlayerError & e )
    {
        std::cerr << e << std::endl;
        cout << "ERROR(playerclientdriver.cpp): Error writing to robot." << endl;
        return -1;
    }
    return 0;     
}
