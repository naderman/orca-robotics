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
using namespace segwayrmp;
using namespace PlayerCc;

// experiment to pass all custom props as a disctionary
//PlayerClientDriver::PlayerClientDriver( const std::map<std::string,std::string> & props, const std::string & prefix )

PlayerClientDriver::PlayerClientDriver( const orcaice::Context & context )
    : enabled_( false ),
      robot_(0),
//       positionProxy_(0),
      position3dProxy_(0),
      powerProxy_(0),
      context_(context)
{
    // parse configuration parameters
    readFromProperties( context, config_ );
    cout<<config_<<endl;
}

PlayerClientDriver::~PlayerClientDriver()
{
    disable();
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
//         positionProxy_ = new PlayerCc::Position2dProxy( robot_, 0 );
        position3dProxy_ = new PlayerCc::Position3dProxy( robot_, 0 );
    
        robot_->Read();
    
//         positionProxy_->SetMotorEnable( true );
        position3dProxy_->SetMotorEnable( true );
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

// int PlayerClientDriver::repair()
// {
//     disable();
//     return enable();
// }

int
PlayerClientDriver::disable()
{
    if ( !enabled_ ) return 0;

//     delete positionProxy_;
    delete position3dProxy_;
    delete powerProxy_;
    delete robot_;
    enabled_ = false;
    return 0;
}

int
PlayerClientDriver::read( SegwayRmpData& data, std::string &status )
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
//     orcaplayer::convert( *position3dProxy_, position3d );
    orca::Time t = orcaice::toOrcaTime( position3dProxy_->GetDataTime() );
    data.seconds = t.seconds;
    data.useconds = t.useconds;

    data.x = position3dProxy_->GetXPos();
    data.y = position3dProxy_->GetYPos();
//     data.z = position3dProxy_->GetZPos();
    data.roll = position3dProxy_->GetRoll();
    data.pitch = position3dProxy_->GetPitch();
    data.yaw = position3dProxy_->GetYaw();
    
    data.vx = position3dProxy_->GetXSpeed();
//     data.vy = position3dProxy_->GetYSpeed();
//     data.vz = position3dProxy_->GetZSpeed();
    data.droll = position3dProxy_->GetRollSpeed();
    data.dpitch = position3dProxy_->GetPitchSpeed();
    data.dyaw = position3dProxy_->GetYawSpeed();

    //orcaplayer::convert( *powerProxy_, power );

    status = "playing=1";

    return 0;
}

int
PlayerClientDriver::write( const SegwayRmpCommand& command )
{
    // this version of Player client takes speed command in  [m, m, rad/s]
    try
    {
        position3dProxy_->SetSpeed( command.vx, 0.0, command.w );
    }
    catch ( const PlayerCc::PlayerError & e )
    {
        std::cerr << e << std::endl;
        cout << "ERROR(playerclientdriver.cpp): Error writing to robot." << endl;
        return -1;
    }
    return 0;     
}
