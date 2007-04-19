/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "playerclientodometry3ddriver.h"
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

namespace imu {

PlayerClientOdometry3dDriver::PlayerClientOdometry3dDriver( const char *host, int port, bool isStage )
    : enabled_( false ),
      host_(strdup(host)),
      port_(port),
      isStage_( isStage )
{
}

PlayerClientOdometry3dDriver::~PlayerClientOdometry3dDriver()
{
}

int
PlayerClientOdometry3dDriver::enable()
{
    if ( enabled_ ) return 0;

    cout << "TRACE(playerclientposition3ddriver.cpp): PlayerClientOdometry3dDriver: Connecting to player on host "
         << host_ << ", port " << port_ << endl;

    // player throws exceptions on creation if we fail
    try
    {
        robot_      = new PlayerCc::PlayerClient( host_, port_ );
        position3dProxy_ = new PlayerCc::Position3dProxy( robot_, 0 );
    
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
PlayerClientOdometry3dDriver::disable()
{
    if ( !enabled_ ) return 0;

    delete position3dProxy_;
    delete robot_;
    enabled_ = false;
    return 0;
}

int
PlayerClientOdometry3dDriver::read()
{
    // newImuData_ is always false since we are only interfacing to position3d provided by gazebo
    newImuData_ = false;
    newOdometry3dData_ = false;

    if ( ! enabled_ )
    {
        cout << "ERROR(playerclientpositoin3ddriver.cpp): Can't read: not connected to Player/Stage yet. Sleeping for 1 sec..." << endl;
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
        cout << "ERROR(playerclientposition3ddriver.cpp): Error reading from robot." << endl;
        enabled_ = false;
        return -1;
    }
    
    // the convert function accepts smart pointers so need to have a temporary variable
    // to hold the converted data.
    // Is there a more efficient way of doing this?
    // orca::Odometry3dDataPtr tmpData;
    orca::Odometry3dData tmpOdometry3dData;
    orcaplayer::convert( *position3dProxy_, tmpOdometry3dData );

    odometry3dData_ = tmpOdometry3dData;
    newOdometry3dData_ = true;

    return 0;
}

int 
PlayerClientOdometry3dDriver::getData(orca::ImuData& data )
{
        // gazebo doesn't provide simulated imu raw data so always return -1
        return -1;

}

int 
PlayerClientOdometry3dDriver::getData(orca::Odometry3dData& data )
{
    if(newOdometry3dData_){
        newOdometry3dData_ = false;
        data = odometry3dData_;
        return 0;
    }else{
        return -1;
    }

}

// int
// PlayerClientOdometry3dDriver::getGeometry( orca::Position3dGeometryPtr& geometry )
// {
    // Avoid a Stage v1.3.5 and v2.0.0 bug...
//     if ( isStage_ )
//     {
//         // default settings
//         cfg->angleIncrement   = DEG2RAD(0.5);
//         cfg->rangeResolution  = 0.001;
//         cfg->maxRange         = 8.0;
//         cfg->isEnabled        = enabled_;
//         return 0;
//     }

//     if ( ! enabled_ )
//     {
//         cout << "ERROR(playerclientposition3ddriver.cpp): Can't read: not connected to Player/Stage yet." << endl;
//         return -1;
//     }
// 
//     try
//     {
//         position3dProxy_->RequestConfigure();
//     }
//     catch ( const PlayerCc::PlayerError & e )
//     {
//         std::cerr << e << std::endl;
//         cout << "ERROR(playerclientposition3ddriver.cpp): Problem getting configuration info." << endl;
//         return -1;
//     }
// 
//     // convert scan and range resolutions
//     orcaplayer::convert( *position3dProxy_, cfg );
//     
//     cfg->isEnabled        = enabled_;
//     
/*    
    cout << "WARNING(playerclientposition3ddriver.cpp): geometry values are hardcoded at the moment! " << endl;
    // orca::Position3dGeometryPtr position3dGeometryPtr = new Position3dGeometry;
    Frame3d offset;
    orcaice::setInit( offset );
    Size3d size;
    size.l=0.3; size.w=0.3; size.h=0.3;
    geometry->offset = offset;
    geometry->size = size;
    // position3dGeometryBuffer_.push( position3dGeometryPtr );
   
    return 0;
}*/

} //namespace
