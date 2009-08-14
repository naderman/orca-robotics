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

namespace bicycleplayerclient 
{

Driver::Config::Config()
{
    host                       = "localhost";
    port                       = 6665;
    usePlayerTimestamp         = false;
    position2dIndex            = 0;
    position1dIndex            = 0;
    simulatedMaxSteerAngleRate = 0;
}


std::string
Driver::Config::toString()
{
    stringstream ss;
    ss<< "PlayerClient Driver Configuration Parameters:"              <<endl
      << "\thost:                       " << host                     <<endl
      << "\tport:                       " << port                     <<endl
      << "\tusePlayerTimestamp:         " << usePlayerTimestamp       <<endl
      << "\tposition2dIndex:            " << position2dIndex          <<endl
      << "\tposition1dIndex:            " << position1dIndex;
      
    if( position1dIndex < 0 )
    {
        // Write additional parameters
        ss<< endl
          << "\tsimulatedMaxSteerAngleRate: " << simulatedMaxSteerAngleRate;
    }

    return ss.str();
}

void
Driver::Config::fromProperties( const hydroutil::Properties& props )
{
    std::string playerHost = props.getPropertyWithDefault( "PlayerClient.Host", "localhost" );
    host = playerHost.c_str();

    port = props.getPropertyAsIntWithDefault( "PlayerClient.Port", 6665 );

    usePlayerTimestamp =
      props.getPropertyAsIntWithDefault( "PlayerClient.UsePlayerTimestamp", 0 );
    
    position2dIndex = props.getPropertyAsIntWithDefault( "PlayerClient.Position2dIndex", 0 );

    std::string aux;
    if( 0 != props.getProperty( "PlayerClient.Position1dIndex", aux ) )
    {
      throw Exception( "Config::fromProperties(): missing property PlayerClient.Position1dIndex" );
    }

    if( aux == std::string( "none" ) )
    {
        position1dIndex = -1;
	simulatedMaxSteerAngleRate =
	    props.getPropertyAsIntWithDefault( "SimulatedMaxSteerAngleRate", 0 );
    }
    else
    {
        position1dIndex = props.getPropertyAsIntWithDefault( "PlayerClient.Position1dIndex", 0 );
    }
}

/////////////////////////////////

Driver::Driver( const hydroutil::Context & context )
    : enabled_( false ),
      simulatedCurrentSteerAngle_( 0 ),
      simulatedTargetSteerAngle_( 0 ),
      context_(context)
{
    int seconds, useconds;
    hydroiceutil::now( seconds, useconds );
    simulatedCurrentSteerAngle_ =
      hydroiceutil::timeAsDouble( seconds, useconds );

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
        robot_.reset( new PlayerCc::PlayerClient( config_.host, config_.port ) );
        positionProxy_.reset( new PlayerCc::Position2dProxy( robot_.get(), config_.position2dIndex ) );
        
        if( config_.position1dIndex >= 0 )
        {
            currentSteerAngleProxy_.reset( new PlayerCc::Position1dProxy( robot_.get(), config_.position1dIndex ) );
        }
    
        robot_->Read();
    
        positionProxy_->SetMotorEnable( true );
    }
    catch ( const PlayerCc::PlayerError & e )
    {
        stringstream ss;
        ss << "Driver: " << e;
        disable();
        throw hydrointerfaces::Bicycle::Exception( ss.str() );
    }

    enabled_ = true;
}

void
Driver::disable()
{
    if ( !enabled_ ) return;

    positionProxy_.reset();
    currentSteerAngleProxy_.reset();
    robot_.reset();
    enabled_ = false;
}

bool
Driver::read( Data& data )
{
    if ( ! enabled_ ) {
        stringstream ss;
        ss << "Driver: Can't read: not connected to Player/Stage yet.";
        throw hydrointerfaces::Bicycle::Exception( ss.str() );
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
        throw hydrointerfaces::Bicycle::Exception( ss.str() );
        enabled_ = false;
    }
    
    // timestamp either comes from Player or we simply use the current time
    if (config_.usePlayerTimestamp)
        hydroiceutil::timeFromDouble( positionProxy_->GetDataTime(), data.seconds, data.useconds );
    else
        hydroiceutil::now( data.seconds, data.useconds );    
    
    data.x = positionProxy_->GetXPos();
    data.y = positionProxy_->GetYPos();
    data.yaw = positionProxy_->GetYaw();
    
    data.vlong = positionProxy_->GetXSpeed();
    data.dyaw = positionProxy_->GetYawSpeed();
    
    // request or simulate the current steer angle
    if( currentSteerAngleProxy_.get() )
    {
        data.steerAngle = currentSteerAngleProxy_->GetPos();
    }
    else
    {
        // no proxy available to request the real steer angle, so simulate it

        // if immediate response was set, just do it
        if( config_.simulatedMaxSteerAngleRate <= 0 )
	{
	    simulatedCurrentSteerAngle_ = simulatedTargetSteerAngle_;
	}
	else
	{
	    // first, calculate the elapsed time since the last read
	    int seconds, useconds;
	    hydroiceutil::now( seconds, useconds );
	    double now = hydroiceutil::timeAsDouble( seconds, useconds );
	    double elapsedTime = now - simulatedCurrentSteerAngleTimestamp_;
	    simulatedCurrentSteerAngleTimestamp_ = now;

	    // update the steer angle
	    double elapsedSteerAngle = config_.simulatedMaxSteerAngleRate * elapsedTime;
	    if( elapsedSteerAngle >= fabs( simulatedTargetSteerAngle_ -
					   simulatedCurrentSteerAngle_ ) )
	    {
		// target angle reached
		simulatedCurrentSteerAngle_ = simulatedTargetSteerAngle_;
	    }
	    else
	    {
		// move towards target steer angle
		double direction =
		    (simulatedTargetSteerAngle_ > simulatedCurrentSteerAngle_)?
		    1.0 : -1.0;
		simulatedCurrentSteerAngle_ += direction * elapsedSteerAngle;
	    }
	}

	// return the simulated steer angle
	data.steerAngle = simulatedCurrentSteerAngle_;
    }

    return false;
}

void
Driver::write( const Command& command )
{
    // this version of Player client takes speed command in [m/s, rad/s]
    try
    {
        // store the steer angle we want to command
        simulatedTargetSteerAngle_ = command.steerAngle;

	// if told to, simulate the steer angle delay in commands
	if( config_.simulatedMaxSteerAngleRate > 0 )
	{
	    // consider read() is being called often enough, so
	    // simulatedCurrentSteerAngle_ should be already updated
	    positionProxy_->SetCarlike( command.vlong,
					simulatedCurrentSteerAngle_ );
	}
	else
	{
	    positionProxy_->SetCarlike( command.vlong, command.steerAngle );
	}
    }
    catch ( const PlayerCc::PlayerError & e )
    {
        stringstream ss;
        ss << "Driver::write(): " << e;
        throw hydrointerfaces::Bicycle::Exception( ss.str() );
    }
}

void 
Driver::getStatus( std::string &status, bool &isWarn, bool &isFault )
{
    status  = "playing=1";
    isWarn  = false;
    isFault = false;
}

} // namespace

extern "C" {
    hydrointerfaces::BicycleFactory *createDriverFactory()
    { return new bicycleplayerclient::Factory; }
}

