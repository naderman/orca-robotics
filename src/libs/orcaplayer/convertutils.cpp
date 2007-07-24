/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <assert.h>
#include <IceUtil/Time.h>
#include <orcaice/orcaice.h>

// Player proxies
#include <libplayerc++/playerc++.h>

#include "convertutils.h"

using namespace std;

namespace orcaplayer
{

void
convert( const PlayerCc::LaserProxy & proxy, orca::LaserScanner2dDataPtr& obj, bool convertHeaders )
{
    obj->timeStamp = orcaice::toOrcaTime( proxy.GetDataTime() );
    
    obj->ranges.resize( proxy.GetCount() );
    obj->intensities.resize( proxy.GetCount() );

    double rangeRes = proxy.GetRangeRes();

    if (rangeRes == 0.0)  // don't let the absence of this cause us to lose data
        rangeRes = 1.0;

    for ( unsigned int i = 0; i < proxy.GetCount(); ++i )
    {
        obj->ranges[i]      = proxy.GetRange(i) * rangeRes;
        // GetIntensity returns a double. is this a bug in player2?
        obj->intensities[i] = (int)floor( proxy.GetIntensity(i) );
    }

    if ( convertHeaders ) {
        int numberOfSamples;
        convert( proxy, obj->maxRange, obj->fieldOfView, obj->startAngle, numberOfSamples );

        // double-check 
        assert ( numberOfSamples != (int)obj->ranges.size() && "mismatch in data sizes" );
    }
}

void
convert( const PlayerCc::LaserProxy & proxy, 
         double &maxRange, double &fieldOfView, double &startAngle, int &numberOfSamples )
{
    // note: GetMaxRange() has disappeared from Player 2.0.3

    // player returns [mm] as double
    double rangeResolution  = (double)proxy.GetRangeRes()/1000.0;

    if ( rangeResolution == 0.01 ) {
        maxRange = 80.0;
    }
    else if ( rangeResolution == 0.001 ) {
        maxRange = 8.0;
    }
    else {   
        // maybe this is not as SICK laser.
        // cannot trust their numbers half the time anyway
        maxRange = 0.0;
    }

    // player returns [rad]
    startAngle = proxy.GetMinAngle();

    fieldOfView = proxy.GetMaxAngle() - startAngle;

    numberOfSamples = proxy.GetCount();

    // player returns [rad]
//     double angleIncrement   = proxy.GetScanRes();
}

void
convert( PlayerCc::SimulationProxy & proxy, orca::Localise2dData& obj, const std::string & id )
// convert( const PlayerCc::SimulationProxy & proxy, orca::Localise2dData& obj, const std::string & id )
{
    obj.timeStamp = orcaice::toOrcaTime( proxy.GetDataTime() );
    
    obj.hypotheses.resize(1);
    obj.hypotheses[0].weight = 1.0;
    char* identifier = strdup( id.c_str() );
    double x,y,theta;
    proxy.GetPose2d( identifier, x, y, theta );
    obj.hypotheses[0].mean.p.x = x;
    obj.hypotheses[0].mean.p.y = y;
    obj.hypotheses[0].mean.o = theta;
    obj.hypotheses[0].cov.xx   = 0.01;
    obj.hypotheses[0].cov.yy   = 0.01;
    obj.hypotheses[0].cov.tt   = 0.01*M_PI/180.0;
    obj.hypotheses[0].cov.xy   = 0.0;
    obj.hypotheses[0].cov.xt   = 0.0;
    obj.hypotheses[0].cov.yt   = 0.0;
}

void convert( const orca::LaserScanner2dDataPtr& obj, PlayerCc::LaserProxy & proxy )
{
    cout<<"!!!!!!!!!!!! NOT IMPLEMENTED !!!!!!!!!!!!!!"<<endl;
    cout<<" orcaplayer::convert( const orca::LaserScanner2dData& obj, PlayerCc::LaserProxy & proxy )"<<endl;
    cout<<"!!!!!!!!!!!! NOT IMPLEMENTED !!!!!!!!!!!!!!"<<endl;
    throw "orcaplayer::convert(laser data) is not implemented";
}

void
convert( const PlayerCc::Position3dProxy & proxy, orca::Odometry3dData& obj )
{
    obj.timeStamp = orcaice::toOrcaTime( proxy.GetDataTime() );

    obj.pose.p.x = proxy.GetXPos();
    obj.pose.p.y = proxy.GetYPos();
    obj.pose.p.z = proxy.GetZPos();
    obj.pose.o.r = proxy.GetRoll();
    obj.pose.o.p = proxy.GetPitch();
    obj.pose.o.y = proxy.GetYaw();
    
    obj.motion.v.x = proxy.GetXSpeed();
    obj.motion.v.y = proxy.GetYSpeed();
    obj.motion.v.z = proxy.GetZSpeed();
    obj.motion.w.x = proxy.GetRollSpeed();
    obj.motion.w.y = proxy.GetPitchSpeed();
    obj.motion.w.z = proxy.GetYawSpeed();

    // orca does not have this field.
    //obj.stalled = proxy.GetStall();
}


} // namespace
