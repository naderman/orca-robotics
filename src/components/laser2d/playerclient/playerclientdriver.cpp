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
PlayerClientDriver::PlayerClientDriver( const Config & cfg, const orcaice::Context & context )
    : Driver(cfg),
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
}

PlayerClientDriver::~PlayerClientDriver()
{
}

int
PlayerClientDriver::init()
{
    std::stringstream ss;
    if ( robot_ || laser_ ) {
        ss << "Re-initializing";
    }
    else {
        ss << "Initializing";
    }
    ss << " playerclient driver with host="<<host_<<" port="<<port_<<" id="<<device_<<" drvr="<<playerDriver_;
    context_.tracer()->info( ss.str() );

    if ( laser_ ) {
        delete laser_;
        laser_ = 0;
    }
    if ( robot_ ) {
        delete robot_;
        robot_ = 0;
    }

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
        context_.tracer()->debug( ss.str(), 3 );
        return -1;
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
        context_.tracer()->debug( ss.str(), 3 );
        return -1;
    }

    // convert scan and range resolutions
    Config playerCfg;
    orcaplayer::convert( *laser_, playerCfg.maxRange, playerCfg.fieldOfView, playerCfg.startAngle, playerCfg.numberOfSamples );

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
            return 1;
        }
    } 
    else if ( playerDriver_=="urglaser" ) {
        // we use configs from our own config file and hope that they match the Player server configs

        if ( config_.numberOfSamples != playerCfg.numberOfSamples ) {
            context_.tracer()->warning( "Config file does not match parameters reported by Player server" );
            // should we update the config structure?
            return 1;
        }
        if ( config_.fieldOfView != playerCfg.numberOfSamples ) {
            context_.tracer()->warning( "Config file does not match parameters reported by Player server" );
            // should we update the config structure?
            return 1;
        }
    } 
    else {        
        if ( config_ != playerCfg ) {
            context_.tracer()->warning( "Config file does not match parameters reported by Player server" );
            // should we update the config structure?
            return 1;
        }
    }
    
    return 0;
}

int
PlayerClientDriver::read( LaserScanner2dDataPtr &data )
{
    if ( !robot_ || !laser_ ) {
        return -1;
    }

    try
    {
        robot_->Read();
    }
    catch ( const PlayerCc::PlayerError & e )
    {
        stringstream ss;
        ss << "Player exception on read: " << e;
        context_.tracer()->debug( ss.str(), 3 );
        return -1;
    }

    // don't convert headers again, we've already done it on initialization
    bool convertHeaders = false;
    orcaplayer::convert( *laser_, data, convertHeaders );

    // copy description from config
    data->minRange    = config_.minRange;
    data->maxRange    = config_.maxRange;
    data->fieldOfView = config_.fieldOfView;
    data->startAngle  = config_.startAngle;

    return 0;
}

} // namespace
