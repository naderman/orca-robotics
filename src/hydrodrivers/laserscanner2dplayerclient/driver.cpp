/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <sstream>
#include <assert.h>
#include <cstring>

#include <gbxutilacfr/exceptions.h>
#include <libplayerc++/playerc++.h>
#include <hydroiceutil/timeutils.h>

#include "driver.h"

using namespace std;
using namespace PlayerCc;

namespace laserscanner2dplayerclient {

/*
  NOTE: as of player v1.5 the LaserProxy returns range in [m], the multiplication factor is no longer needed.
*/
Driver::Driver( const Config &cfg, const hydroutil::Context &context )
    : config_(cfg),
      robot_(0),
      laser_(0),
      context_(context)
{
    // read driver-specific properties
    std::string prefix = "PlayerClient.";
    
    usePlayerTimestamp_ = context_.properties().getPropertyAsIntWithDefault( prefix+"UsePlayerTimestamp", 0 );

    std::string playerHost = context_.properties().getPropertyWithDefault( prefix+"Host", "localhost" );
    host_ = strdup(playerHost.c_str());

    port_ = context_.properties().getPropertyAsIntWithDefault( prefix+"Port", 6665 );

    device_ = context_.properties().getPropertyAsIntWithDefault( prefix+"Device", 0 );

    playerDriver_ = context_.properties().getPropertyWithDefault( prefix+"Driver", "sicklms200" );

    init();
}

Driver::~Driver()
{
    if ( laser_ ) delete laser_;
    if ( robot_ ) delete robot_;
}

void
Driver::init()
{
    std::stringstream ss;
    ss << "initializing";
    ss << " playerclient driver with host="<<host_<<" port="<<port_<<" id="<<device_<<" drvr="<<playerDriver_;
    context_.tracer().info( ss.str() );

    // player throws exceptions on creation if we fail
    try
    {
        robot_ = new PlayerCc::PlayerClient( host_, port_ );
        laser_ = new PlayerCc::LaserProxy( robot_, device_ );
    
        // try to read once
        robot_->Read();
    }
    catch ( const PlayerCc::PlayerError & e )
    {
        stringstream ss;
        ss << "Player exception on init: " << e;
        throw ss.str();
//         throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }

    // we  are left with sicklms200 (real hardware) for which we can get live config data.
    context_.tracer().info("Player driver is '"+playerDriver_+"'. Verifying configuration...");
    try
    {
        laser_->RequestConfigure();
    }
    catch ( const PlayerCc::PlayerError & e )
    {
        stringstream ss;
        ss << "Player exception on get config: " << e;
        throw ss.str();
    }

    // convert scan and range resolutions
    Config playerCfg;

    // player returns [mm] as double
    double rangeResolution  = (double)laser_->GetRangeRes()/1000.0;

    // note: GetMaxRange() has disappeared from Player 2.0.3
    if ( rangeResolution == 0.01 ) {
        playerCfg.maxRange = 80.0;
    }
    else if ( rangeResolution == 0.001 ) {
        playerCfg.maxRange = 8.0;
    }
    else {   
        // maybe this is not a SICK laser.
        // cannot trust their numbers half the time anyway
        playerCfg.maxRange = 0.0;
    }

    // player returns [rad]
    playerCfg.startAngle = laser_->GetMinAngle();

    playerCfg.fieldOfView = laser_->GetMaxAngle() - playerCfg.startAngle;

    playerCfg.numberOfSamples = laser_->GetCount();

    ss.str( "" );
    ss << "Config info from Player: maxrange="<<playerCfg.maxRange<<" fov="<<playerCfg.fieldOfView<<" start="<<playerCfg.startAngle<<" num="<<playerCfg.numberOfSamples;
    context_.tracer().info( ss.str() );

    // some Player drivers do not implement config requests properly
    // we verify some properties. for the rest we use configs from our own config file and 
    // hope that they match the Player server configs
    if ( playerDriver_=="stage" || playerDriver_=="gazebo" ) {

        if ( config_.numberOfSamples != playerCfg.numberOfSamples ) {
            context_.tracer().warning( "Config file does not match parameters reported by Player server" );
            // should we update the config structure?
            return ;
        }
    } 
    else if ( playerDriver_=="urglaser" ) {
        // we use configs from our own config file and hope that they match the Player server configs

        if ( config_.numberOfSamples != playerCfg.numberOfSamples ) {
            context_.tracer().warning( "Config file does not match parameters reported by Player server" );
            // should we update the config structure?
            return;
        }
        if ( config_.fieldOfView != playerCfg.numberOfSamples ) {
            context_.tracer().warning( "Config file does not match parameters reported by Player server" );
            // should we update the config structure?
            return;
        }
    } 
    else {        
        if ( config_ != playerCfg ) {
            context_.tracer().warning( "Config file does not match parameters reported by Player server" );
            // should we update the config structure?
            return;
        }
    }
}

void
Driver::read( hydrointerfaces::LaserScanner2d::Data &data )
{
    assert( robot_ && laser_ );

    try
    {
        robot_->Read();
    }
    catch ( const PlayerCc::PlayerError & e )
    {
        stringstream ss;
        ss << "Player exception on read: " << e;
        throw ss.str();
    }

    // don't convert headers again, we've already done it on initialization
    if ( (int)(laser_->GetCount()) != config_.numberOfSamples )
    {
        stringstream ss;
        ss << "Driver::read(): number of samples returned by player ("<<laser_->GetCount()<<") != number we expect ("<<config_.numberOfSamples<<")";
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }
    for ( unsigned int i = 0; i < laser_->GetCount(); ++i )
    {
        data.ranges[i]      = laser_->GetRange(i);
        // GetIntensity returns a double. is this a bug in player2?
        data.intensities[i] = (int)floor( laser_->GetIntensity(i) );
    }

    // timestamp either comes from Player or we simply use the current time
    if (usePlayerTimestamp_)
        hydroiceutil::timeFromDouble( laser_->GetDataTime(), data.timeStampSec, data.timeStampUsec );
    else
        hydroiceutil::now( data.timeStampSec, data.timeStampUsec );    
}

} // namespace

extern "C" {
    hydrointerfaces::LaserScanner2dFactory *createDriverFactory()
    { return new laserscanner2dplayerclient::Factory; }
}
