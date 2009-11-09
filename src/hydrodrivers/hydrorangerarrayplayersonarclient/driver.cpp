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

#include <hydroutil/hydroutil.h>
#include <libplayerc++/playerc++.h>
#include <hydroiceutil/timeutils.h>

#include "driver.h"

using namespace std;
using namespace PlayerCc;

namespace rangerarrayplayersonarclient {

Driver::Driver( const hydroutil::Context &context )
    : robot_(0),
      sonar_(0),
      context_(context)
{
    // read driver-specific properties
    std::string prefix = "PlayerSonarClient.";
    
    usePlayerTimestamp_ = context_.properties().getPropertyAsIntWithDefault( prefix+"UsePlayerTimestamp", 0 );
    detectionConeAngle_ = context_.properties().getPropertyAsDoubleWithDefault( prefix+"DetectionConeAngle", 0 ) * DEG2RAD_RATIO;

    std::string playerHost = context_.properties().getPropertyWithDefault( prefix+"Host", "localhost" );
    host_ = strdup(playerHost.c_str());

    port_ = context_.properties().getPropertyAsIntWithDefault( prefix+"Port", 6665 );

    device_ = context_.properties().getPropertyAsIntWithDefault( prefix+"Device", 0 );

    init();
}

Driver::~Driver()
{
    if ( sonar_ ) delete sonar_;
    if ( robot_ ) delete robot_;
}

void
Driver::init()
{
    std::stringstream ss;
    ss << "initializing";
    ss << " playersonarclient driver with host="<<host_<<" port="<<port_<<" id="<<device_;
    context_.tracer().info( ss.str() );

    // player throws exceptions on creation if we fail
    try
    {
        robot_ = new PlayerCc::PlayerClient( host_, port_ );
        sonar_ = new PlayerCc::SonarProxy( robot_, device_ );
    
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
}

void
Driver::getDescription( hydrointerfaces::RangerArray::Description &description )
{
    assert( robot_ && sonar_ );

    try
    {
        sonar_->RequestGeom();
    }
    catch ( const PlayerCc::PlayerError & e )
    {
        stringstream ss;
        ss << "Player exception on getGeometry: " << e;
        throw ss.str();
    }
    
    description.minRange = 0;
    description.maxRange = 0;
    description.detectionConeAngle = detectionConeAngle_;
    description.poses.clear();
    for ( unsigned int i = 0; i < sonar_->GetCount(); ++i )
    {
	player_pose3d_t pose = sonar_->GetPose(i);
	description.poses.push_back( hydrobros1::Frame3d( pose.px, pose.py, pose.pz, pose.proll, pose.ppitch, pose.pyaw ) );
    }
}

void
Driver::read( hydrointerfaces::RangerArray::Data &data )
{
    assert( robot_ && sonar_ );

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

    // timestamp either comes from Player or we simply use the current time
    int timeStampSec, timeStampUsec;
    if (usePlayerTimestamp_)
        hydroiceutil::timeFromDouble( sonar_->GetDataTime(), timeStampSec, timeStampUsec );
    else
        hydroiceutil::now( timeStampSec, timeStampUsec );

    for ( unsigned int i = 0; i < sonar_->GetCount(); ++i )
    {
        data.ranges[i] = sonar_->GetScan(i);

        // use the same timestamp for every scan, as Player does not give individual information
        data.timeStampsSec[i] = timeStampSec;
        data.timeStampsUsec[i] = timeStampUsec;
    }

}

} // namespace

extern "C" {
    hydrointerfaces::RangerArrayFactory *createRangerArrayDriverFactory()
    { return new rangerarrayplayersonarclient::Factory; }
}
