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

namespace robot2dplayerclient 
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
      positionProxy_(0),
      powerProxy_(0),
      context_(context)
{
    usePlayerTimestamp_ = context_.properties().getPropertyAsIntWithDefault( "PlayerClient.UsePlayerTimestamp", 0 );
    
    // parse configuration parameters
    config_.fromProperties( context_.properties() );
    // nothing to verify right now
//     config_.verify();
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
        positionProxy_ = new PlayerCc::Position2dProxy( robot_, 0 );
    
        robot_->Read();
    
        positionProxy_->SetMotorEnable( true );
    }
    catch ( const PlayerCc::PlayerError & e )
    {
        stringstream ss;
        ss << "Driver: " << e;
        disable();
        throw hydrointerfaces::Robot2d::Exception( ss.str() );
    }

    enabled_ = true;
}

void
Driver::disable()
{
    if ( !enabled_ ) return;

    delete positionProxy_;
    delete powerProxy_;
    delete robot_;
    enabled_ = false;
}

bool
Driver::read( Data& data )
{
    if ( ! enabled_ ) {
        stringstream ss;
        ss << "Driver: Can't read: not connected to Player/Stage yet.";
        throw hydrointerfaces::Robot2d::Exception( ss.str() );
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
        throw hydrointerfaces::Robot2d::Exception( ss.str() );
        enabled_ = false;
    }
    
    // timestamp either comes from Player or we simply use the current time
    if (usePlayerTimestamp_)
        hydroiceutil::timeFromDouble( positionProxy_->GetDataTime(), data.seconds, data.useconds );
    else
        hydroiceutil::now( data.seconds, data.useconds );    
    
    data.x = positionProxy_->GetXPos();
    data.y = positionProxy_->GetYPos();
    data.yaw = positionProxy_->GetYaw();
    
    data.vlong = positionProxy_->GetXSpeed();
    data.vtransverse = positionProxy_->GetYSpeed();
    data.dyaw = positionProxy_->GetYawSpeed();

    return false;
}

void
Driver::write( const Command& command )
{
    // this version of Player client takes speed command in  [m, m, rad/s]
    try
    {
        positionProxy_->SetSpeed( command.vlong, command.vtransverse, command.dyaw );
    }
    catch ( const PlayerCc::PlayerError & e )
    {
        stringstream ss;
        ss << "Driver::write(): " << e;
        throw hydrointerfaces::Robot2d::Exception( ss.str() );
    }
}

void 
Driver::getStatus( std::string &status, bool &isWarn, bool &isFault )
{
    status  = "playing=1";
    isWarn  = false;
    isFault = false;
}
void 
Driver::applyHardwareLimits( double &maxForwardSpeed, double &maxReverseSpeed, 
                              double &maxTurnrate, double &maxTurnrateAtMaxSpeed )
{
}

} // namespace

extern "C" {
    hydrointerfaces::Robot2dFactory *createDriverFactory()
    { return new robot2dplayerclient::Factory; }
}

