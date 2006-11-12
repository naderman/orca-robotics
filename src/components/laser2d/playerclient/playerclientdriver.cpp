/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "playerclientdriver.h"
#include <iostream>
#include <stdlib.h>
#include <assert.h>

#include <IceUtil/Thread.h>     // for sleep()

#include <orcaice/orcaice.h>    // for mathdef.h only
#include <libplayerc++/playerc++.h>
#include <orcaplayer/orcaplayer.h>

using namespace std;
using namespace orca;
using namespace PlayerCc;

namespace sicklaser {

/*
  NOTE: as of player v1.5 the LaserProxy returns range in [m], the multiplication factor is no longer needed.
*/
PlayerClientDriver::PlayerClientDriver( const char *host, int port, int device, const std::string & playerDriver )
    : enabled_( false ),
      host_(strdup(host)),
      port_(port),
      device_(device),
      playerDriver_( playerDriver )
{
}

PlayerClientDriver::~PlayerClientDriver()
{
}

int
PlayerClientDriver::enable()
{
    if ( enabled_ ) return 0;

    cout << "TRACE(playerlaserdriver.cpp): PlayerClientDriver: Connecting to player on host "
         << host_ << ", port " << port_ << endl;

    // player throws exceptions on creation if we fail
    try
    {
        robot_      = new PlayerCc::PlayerClient( host_, port_ );
        laserProxy_ = new PlayerCc::LaserProxy( robot_, device_ );
    
        robot_->Read();
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

int
PlayerClientDriver::disable()
{
    if ( !enabled_ ) return 0;

    delete laserProxy_;
    delete robot_;
    enabled_ = false;
    return 0;
}


int
PlayerClientDriver::getConfig( RangeScanner2dConfigPtr &cfg )
{
    // some Player drivers do not implement config requests.
    // they have to be hard-wired here.
    if ( playerDriver_=="stage" || playerDriver_=="gazebo" )
    {
        // default settings
//         cfg->angleIncrement   = DEG2RAD(0.5);
//         cfg->rangeResolution  = 0.001;
        cfg->maxRange         = 8.0;
        cfg->isEnabled        = enabled_;
        return 0;
    } 
    else if ( playerDriver_=="urglaser" )
    {
        // default settings
//         cfg->angleIncrement   = DEG2RAD(230.0/654.0);
//         cfg->rangeResolution  = 0.01;
        cfg->maxRange         = 4.0;
        cfg->isEnabled        = enabled_;
        return 0;
    } 

    if ( ! enabled_ )
    {
        cout << "ERROR(playerlaserdriver.cpp): Can't read: not connected to Player/Stage yet." << endl;
        return -1;
    }

    try
    {
        laserProxy_->RequestConfigure();
    }
    catch ( const PlayerCc::PlayerError & e )
    {
        std::cerr << e << std::endl;
        cout << "ERROR(playerlaserdriver.cpp): Problem getting configuration info." << endl;
        return -1;
    }

    // convert scan and range resolutions
    orcaplayer::convert( *laserProxy_, cfg );
    
    cfg->isEnabled        = enabled_;
    
    return 0;
}

int
PlayerClientDriver::setConfig( const RangeScanner2dConfigPtr &cfg )
{
    // handle enable/disable
    if ( !cfg->isEnabled )
    {
        return disable();
    }
    if ( !enabled_ && cfg->isEnabled )
    {
        if ( enable() != 0 )
        {
            return -1;
        }
    }
    
    // only sicklms200 driver supports remote configuration
    if ( playerDriver_!="sicklms200" ) {
        return 0;
    }

    cout<<"TRACE(playerlaserdriver.cpp): Before configuring: " << endl;
    //laserProxy_->PrintConfig();

//     cout<<"TRACE(playerlaserdriver.cpp): Configuring with " << (RAD2DEG(cfg->angleIncrement)*100) << " and " << cfg->rangeResolution*1000 << endl;
    
    try
    {
//alexm todo:
//         laserProxy_->Configure( -M_PI/2,
//                                   M_PI/2,
//                                   (unsigned int) (rint(RAD2DEG(cfg->angleIncrement)*100)),
//                                   (unsigned int) (rint(cfg->rangeResolution*1000)),
//                                   true );
    }
    catch ( const PlayerCc::PlayerError & e )
    {
        std::cerr << e << std::endl;
        cout << "ERROR(playerlaserdriver.cpp): Error configuring laser." << endl;
        return -1;
    }

    try
    {
        laserProxy_->RequestConfigure();
    }
    catch ( const PlayerCc::PlayerError & e )
    {
        std::cerr << e << std::endl;
        cout << "ERROR(playerlaserdriver.cpp): Couldn't read back configuration." << endl;
        return -1;
    }
    
    //laserProxy_->PrintConfig();
//alexm todo:
//     if ( (int) (rint(RAD2DEG(cfg->angleIncrement)*100)) == (int) (rint(RAD2DEG(laserProxy_->GetScanRes())*100)) &&
//          (int) (rint(cfg->rangeResolution*1000))        == laserProxy_->GetRangeRes() )
//     {
//         return 0;
//     }
//     else
//     {
//         cout << "ERROR(playerlaserdriver.cpp): Configuration that we read back doesn't match what we gave it!" << endl;
//         cout<<"TRACE(playerlaserdriver.cpp): either " << (int) (rint(RAD2DEG(cfg->angleIncrement)*100))  <<"!="<< (int) (rint(RAD2DEG(laserProxy_->GetScanRes())*100)) << endl;
//         cout<<"TRACE(playerlaserdriver.cpp): or     " << (int) (rint(cfg->rangeResolution*1000)) <<"!="<< laserProxy_->GetRangeRes() << endl;
// 
//         return -1;
//     }
    return 0;
}

int
PlayerClientDriver::read( LaserScanner2dDataPtr &data )
{
    // debug
    cout<<"PlayerClientDriver::read()"<<endl;

    if ( ! enabled_ )
    {
        cout << "ERROR(playerlaserdriver.cpp): Can't read: not connected to Player/Stage yet. Sleeping for 1 sec..." << endl;
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
        return -1;
    }

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

    orcaplayer::convert( *laserProxy_, data );
    // simulators do not report correct angles
    if ( playerDriver_=="stage" | playerDriver_=="gazebo" )
    {
        // default settings
        data->startAngle       = DEG2RAD(-90.0);
        data->angleIncrement   = DEG2RAD(0.5);
    }
    else  if ( playerDriver_=="urglaser" )
    {
        // default settings
        data->startAngle       = DEG2RAD(-115.0);
        data->angleIncrement   = DEG2RAD(230.0/654.0);
    }

    return 0;
}

}
