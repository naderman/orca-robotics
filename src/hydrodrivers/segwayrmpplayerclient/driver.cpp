/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <assert.h>

#include <libplayerc++/playerc++.h>
#include <hydroiceutil/timeutils.h>

#include "driver.h"

using namespace std;
using namespace PlayerCc;

namespace segwayrmpplayerclient 
{

Driver::Config::Config()
{
    host            = "localhost";
    port            = 6665;
}

int
Driver::Config::verify( std::string &warnings, std::string &errors )
{
    std::stringstream ssWarn, ssErr;

    warnings = ssWarn.str();
    errors   = ssErr.str();

    if ( warnings == "" && errors == "" ) return 0;
    return -1;
}

std::string
Driver::Config::toString()
{
    stringstream ss;
    ss<< "PlayerClient Driver Configuration Parameters:"              <<endl
      << "\thost:              " << host                              <<endl
      << "\tport:              " << port;

    return ss.str();
}

void
Driver::Config::fromProperties( const hydroutil::Properties& props )
{
    std::string playerHost = props.getPropertyWithDefault( "PlayerClient.Host", "localhost" );
    host = playerHost.c_str();

    port = props.getPropertyAsIntWithDefault( "PlayerClient.Port", 6665 );
}

/////////////////////////////////

Driver::Driver( const hydroutil::Context & context )
    : enabled_( false ),
      robot_(0),
      position2dProxy_(0),
      position3dProxy_(0),
      powerProxy_(0),
      context_(context)
{
    // parse configuration parameters
    config_.fromProperties( context_.properties() );
    cout<<config_.toString()<<endl;
}

Driver::~Driver()
{
    disable();
}

void
Driver::enable()
{
    if ( enabled_ ) return;

    cout << "TRACE(playerclientdriver.cpp): Driver: Connecting to player on host "
         << config_.host << ", port " << config_.port << endl;
    
    // player throws exceptions on creation if we fail
    try
    {
        robot_      = new PlayerCc::PlayerClient( config_.host, config_.port );
    }
    catch ( const PlayerCc::PlayerError & e )
    {
        stringstream ss;
        ss << "Driver: " << e;
        disable();
        throw hydrointerfaces::SegwayRmp::Exception( ss.str() );
    }

    try {
        position3dProxy_ = new PlayerCc::Position3dProxy( robot_, 0 );
        position3dProxy_->SetMotorEnable( true );
    } 
    catch ( PlayerCc::PlayerError & e )
    {
        stringstream ss;
        ss << "Driver: failed to connect to position3d: " << e << ", trying position2d";
        context_.tracer().warning( ss.str() );

        try {
            position2dProxy_ = new PlayerCc::Position2dProxy( robot_, 0 );
            position2dProxy_->SetMotorEnable( true );
        }
        catch ( const PlayerCc::PlayerError & e )
        {
            stringstream ss;
            ss << "Driver: Failed to set up position2d" << e;
            disable();
            throw hydrointerfaces::SegwayRmp::Exception( ss.str() );
        }
    }
    assert( position2dProxy_ || position3dProxy_ );
    
    try {
        robot_->Read();
    }
    catch ( const PlayerCc::PlayerError & e )
    {
        stringstream ss;
        ss << "Driver: " << e;
        disable();
        throw hydrointerfaces::SegwayRmp::Exception( ss.str() );
    }
    enabled_ = true;
}

void
Driver::disable()
{
    if ( !enabled_ ) return;

    delete position2dProxy_;
    delete position3dProxy_;
    delete powerProxy_;
    delete robot_;
    enabled_ = false;
}

void
Driver::read( Data& data )
{
    if ( ! enabled_ ) {
        stringstream ss;
        ss << "Driver: Can't read: not connected to Player/Stage yet.";
        throw hydrointerfaces::SegwayRmp::Exception( ss.str() );
    }

    // player throws exceptions on creation if we fail
    try
    {
        robot_->Read();
    }
    catch ( const PlayerCc::PlayerError & e )
    {
        stringstream ss;
        ss << "Driver::read() Error reading from robot: " << e;
        throw hydrointerfaces::SegwayRmp::Exception( ss.str() );
        enabled_ = false;
    }

    if ( position2dProxy_ )
    {
        hydroiceutil::timeFromDouble( position2dProxy_->GetDataTime(), data.seconds, data.useconds );

        data.x     = position2dProxy_->GetXPos();
        data.y     = position2dProxy_->GetYPos();
        data.roll  = 0.0;
        data.pitch = 0.0;
        data.yaw   = position2dProxy_->GetYaw();
    
        data.vx     = position2dProxy_->GetXSpeed();
        data.droll  = 0.0;
        data.dpitch = 0.0;
        data.dyaw   = position2dProxy_->GetYawSpeed();
    }
    else if ( position3dProxy_ )
    {
        data.x     = position3dProxy_->GetXPos();
        data.y     = position3dProxy_->GetYPos();
        data.roll  = position3dProxy_->GetRoll();
        data.pitch = position3dProxy_->GetPitch();
        data.yaw   = position3dProxy_->GetYaw();
    
        data.vx     = position3dProxy_->GetXSpeed();
        data.droll  = position3dProxy_->GetRollSpeed();
        data.dpitch = position3dProxy_->GetPitchSpeed();
        data.dyaw   = position3dProxy_->GetYawSpeed();
    }
    else
    {
        assert( false && "We need some kinda position proxy" );
    }

    // These are unknown, so they get bogus values
    data.leftTorque = 0;
    data.rightTorque = 0;
    data.mainvolt = 80;
    data.uivolt = 9;

    data.hasWarnings = false;
    data.hasFaults   = false;
}

void
Driver::write( const Command& command )
{
    // this version of Player client takes speed command in  [m, m, rad/s]
    try
    {
        if ( position2dProxy_ )
            position2dProxy_->SetSpeed( command.vx, 0.0, command.w );
        else
            position3dProxy_->SetSpeed( command.vx, 0.0, command.w );
    }
    catch ( const PlayerCc::PlayerError & e )
    {
        stringstream ss;
        ss << "Driver::write(): " << e;
        throw hydrointerfaces::SegwayRmp::Exception( ss.str() );
    }
}

hydrointerfaces::SegwayRmp::Capabilities
Driver::capabilities() const
{
    // Player-bots are pretty high-spec'ed...
    hydrointerfaces::SegwayRmp::Capabilities c;
    c.maxForwardSpeed        = 5.0;
    c.maxReverseSpeed        = 5.0;
    c.maxTurnrate            = 999;
    c.maxLateralAcceleration = 3.0;
    return c;
}

} // namespace

extern "C" {
    hydrointerfaces::SegwayRmpFactory *createDriverFactory()
    { return new segwayrmpplayerclient::Factory; }
}

