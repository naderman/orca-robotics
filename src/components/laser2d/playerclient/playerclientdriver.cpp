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
// #include <stdlib.h>
#include <assert.h>

#include <orcaice/orcaice.h>    // for mathdef.h only
#include <libplayerc++/playerc++.h>
#include <orcaplayer/orcaplayer.h>

#include "playerclientdriver.h"

using namespace std;
using namespace orca;
using namespace PlayerCc;

namespace laser2d {

/*
  NOTE: as of player v1.5 the LaserProxy returns range in [m], the multiplication factor is no longer needed.
*/
PlayerClientDriver::PlayerClientDriver( const Config &cfg, 
                                        const orcaice::Context &context )
    : config_(cfg),
      robot_(0),
      laser_(0),
      context_(context)
{
    // read driver-specific properties from config file
    // this only needs to be done once
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag()+".Config.PlayerClient.";

    std::string playerHost = orcaice::getPropertyWithDefault( prop, prefix+"Host", "localhost" );
    host_ = strdup(playerHost.c_str());

    port_ = orcaice::getPropertyAsIntWithDefault( prop, prefix+"Port", 6665 );

    device_ = orcaice::getPropertyAsIntWithDefault( prop, prefix+"Device", 0 );

    playerDriver_ = orcaice::getPropertyWithDefault( prop, prefix+"Driver", "sicklms200" );

    init();
}

PlayerClientDriver::~PlayerClientDriver()
{
    if ( laser_ ) delete laser_;
    if ( robot_ ) delete robot_;
}

void
PlayerClientDriver::init()
{
    std::stringstream ss;
    ss << "initializing";
    ss << " playerclient driver with host="<<host_<<" port="<<port_<<" id="<<device_<<" drvr="<<playerDriver_;
    context_.tracer()->info( ss.str() );

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
        throw hydroutil::Exception( ERROR_INFO, ss.str() );
    }

    // we  are left with sicklms200 (real hardware) for which we can get live config data.
    context_.tracer()->info("Player driver is '"+playerDriver_+"'. Verifying configuration...");
    try
    {
        laser_->RequestConfigure();
    }
    catch ( const PlayerCc::PlayerError & e )
    {
        stringstream ss;
        ss << "Player exception on get config: " << e;
        throw hydroutil::Exception( ERROR_INFO, ss.str() );
    }

    // convert scan and range resolutions
    Config playerCfg;
    orcaplayer::convert( *laser_,
                         playerCfg.maxRange,
                         playerCfg.fieldOfView,
                         playerCfg.startAngle,
                         playerCfg.numberOfSamples );

    ss.str( "" );
    ss << "Config info from Player: maxrange="<<playerCfg.maxRange<<" fov="<<playerCfg.fieldOfView<<" start="<<playerCfg.startAngle<<" num="<<playerCfg.numberOfSamples;
    context_.tracer()->info( ss.str() );

    // some Player drivers do not implement config requests properly
    // we verify some properties. for the rest we use configs from our own config file and 
    // hope that they match the Player server configs
    if ( playerDriver_=="stage" || playerDriver_=="gazebo" ) {

        if ( config_.numberOfSamples != playerCfg.numberOfSamples ) {
            context_.tracer()->warning( "Config file does not match parameters reported by Player server" );
            // should we update the config structure?
            return ;
        }
    } 
    else if ( playerDriver_=="urglaser" ) {
        // we use configs from our own config file and hope that they match the Player server configs

        if ( config_.numberOfSamples != playerCfg.numberOfSamples ) {
            context_.tracer()->warning( "Config file does not match parameters reported by Player server" );
            // should we update the config structure?
            return;
        }
        if ( config_.fieldOfView != playerCfg.numberOfSamples ) {
            context_.tracer()->warning( "Config file does not match parameters reported by Player server" );
            // should we update the config structure?
            return;
        }
    } 
    else {        
        if ( config_ != playerCfg ) {
            context_.tracer()->warning( "Config file does not match parameters reported by Player server" );
            // should we update the config structure?
            return;
        }
    }
}

void
PlayerClientDriver::read( std::vector<float> &ranges, 
                          std::vector<unsigned char> &intensities, 
                          orca::Time &timeStamp )
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
        throw hydroutil::Exception( ERROR_INFO, ss.str() );
    }

    // don't convert headers again, we've already done it on initialization
    if ( (int)(laser_->GetCount()) != config_.numberOfSamples )
    {
        stringstream ss;
        ss << "PlayerClientDriver::read(): number of samples returned by player ("<<laser_->GetCount()<<") != number we expect ("<<config_.numberOfSamples<<")";
        throw hydroutil::Exception( ERROR_INFO, ss.str() );
    }
    for ( unsigned int i = 0; i < laser_->GetCount(); ++i )
    {
        ranges[i]      = laser_->GetRange(i);
        // GetIntensity returns a double. is this a bug in player2?
        intensities[i] = (int)floor( laser_->GetIntensity(i) );
    }
    timeStamp = orcaice::toOrcaTime( laser_->GetDataTime() );
}

} // namespace

extern "C" {
    laser2d::DriverFactory *createDriverFactory()
    { return new laser2d::PlayerClientDriverFactory; }
}
