/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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
    ss << "Initializing playerclient driver with host="<<host_<<" port="<<port_<<" id="<<device_<<" dvr="<<playerDriver_;
    context_.tracer()->info( ss.str() );

    // player throws exceptions on creation if we fail
    try
    {
        robot_      = new PlayerCc::PlayerClient( host_, port_ );
        laserProxy_ = new PlayerCc::LaserProxy( robot_, device_ );
    
        // read once
        robot_->Read();
    }
    catch ( const PlayerCc::PlayerError & e )
    {
        std::cerr << e << std::endl;
        cout << "ERROR(playerclientdriver.cpp): player error" << endl;
        return -1;
    }
    
    // some Player drivers do not implement config requests.
    // we use configs from our own config file and hope that they match the Player server configs
    if ( playerDriver_=="stage" || playerDriver_=="gazebo" )
    {
        context_.tracer()->debug("Player driver is 'stage' or 'gazebo'. Not verifying config.",2);
        return 0;
    } 
    else if ( playerDriver_=="urglaser" )
    {
        context_.tracer()->debug("Player driver is 'urglaser'. Not verifying config.",2);
        return 0;
    } 

    // we  are left with sicklms200 (real hardware) for which we can get live config data.
    try
    {
        laserProxy_->RequestConfigure();
    }
    catch ( const PlayerCc::PlayerError & e )
    {
        std::cerr << e << std::endl;
        context_.tracer()->warning( "Problem getting configuration info from Player." );
        return -1;
    }

    // convert scan and range resolutions
    Config playerCfg;
    orcaplayer::convert( *laserProxy_, playerCfg.maxRange, playerCfg.fieldOfView, playerCfg.startAngle, playerCfg.numberOfSamples );

    ss.clear();
    ss << "Config info from Player: maxrange="<<playerCfg.maxRange<<" fov="<<playerCfg.fieldOfView<<" start="<<playerCfg.startAngle<<" num="<<playerCfg.numberOfSamples;
    context_.tracer()->info( ss.str() );

    if ( playerCfg != config_ ) {
        context_.tracer()->error( "Config file does not match parameters reported by Player server" );
        return 1;
    }

    
    return 0;
}

int
PlayerClientDriver::read( LaserScanner2dDataPtr &data )
{
    try
    {
        robot_->Read();
    }
    catch ( const PlayerCc::PlayerError & e )
    {
        std::cerr << e << std::endl;
        cout << "ERROR(playerclientdriver.cpp): Error reading from robot." << endl;
        return -1;
    }

    orcaplayer::convert( *laserProxy_, data );

    // simulators do not report correct angles
    if ( playerDriver_=="stage" | playerDriver_=="gazebo" ) //|| playerDriver_=="urglaser" )
    {
        // default settings
        data->minRange         = config_.minRange;
        data->maxRange         = config_.maxRange;
        data->fieldOfView      = config_.fieldOfView;
        data->startAngle       = config_.startAngle;
    }

    return 0;
}

} // namespace
