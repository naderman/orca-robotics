/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
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
#include <rmpexception.h>

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

void
PlayerClientDriver::enable()
{
    IceUtil::Mutex::Lock lock(mutex_);

    if ( enabled_ ) return;

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
        stringstream ss;
        ss << "PlayerClientDriver: " << e;
        disable();
        throw RmpException( ss.str() );
    }

    enabled_ = true;
}

void
PlayerClientDriver::disable()
{
    if ( !enabled_ ) return;

//     delete positionProxy_;
    delete position3dProxy_;
    delete powerProxy_;
    delete robot_;
    enabled_ = false;
}

bool
PlayerClientDriver::read( Data& data )
{
    IceUtil::Mutex::Lock lock(mutex_);

    if ( ! enabled_ ) {
        stringstream ss;
        ss << "PlayerClientDriver: Can't read: not connected to Player/Stage yet.";
        throw RmpException( ss.str() );
    }

    // player throws exceptions on creation if we fail
    try
    {
        robot_->Read();
    }
    catch ( const PlayerCc::PlayerError & e )
    {
        stringstream ss;
        ss << "PlayerClientDriver::read() Error reading from robot: " << e;
        throw RmpException( ss.str() );
        enabled_ = false;
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

    return false;
}

void
PlayerClientDriver::write( const Command& command )
{
    IceUtil::Mutex::Lock lock(mutex_);

    // this version of Player client takes speed command in  [m, m, rad/s]
    try
    {
        position3dProxy_->SetSpeed( command.vx, 0.0, command.w );
    }
    catch ( const PlayerCc::PlayerError & e )
    {
        stringstream ss;
        ss << "PlayerClientDriver::write(): " << e;
        throw RmpException( ss.str() );
    }
}

void 
PlayerClientDriver::getStatus( std::string &status, bool &isWarn, bool &isFault )
{
    status  = "playing=1";
    isWarn  = false;
    isFault = false;
}
