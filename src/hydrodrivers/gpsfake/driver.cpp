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
#include <gbxutilacfr/mathdefs.h>
#include <IceUtil/Thread.h>
#include <hydroiceutil/timeutils.h>
#include <gbxutilacfr/exceptions.h>

#include "driver.h"

using namespace std;

namespace gpsfake {

Driver::Driver( const Config &cfg, const hydroutil::Context &context ) :
    numReads_(0),
    dataRecord_(0),
    fixCounter_(0),
    config_(cfg),
    context_(context)
{
    context_.tracer().info( "Initializing with configs:\n"+context_.properties().toString() );

    std::string prefix = "Fake.";

    neverSeeSatellites_ = context_.properties().getPropertyAsIntWithDefault( prefix+"NeverSeeSatellites", 0 );

    // defaults
    latitudes_.push_back( -35.0 );
    latitudes_.push_back( -35.5 );
    latitudes_ = context_.properties().getPropertyAsDoubleVectorWithDefault( prefix+"Latitudes", latitudes_ );

    // defaults
    longitudes_.push_back( 150.0 );
    longitudes_.push_back( 149.5 );
    longitudes_ = context_.properties().getPropertyAsDoubleVectorWithDefault( prefix+"Longitudes", longitudes_ );

    if ( latitudes_.size() != longitudes_.size() )
        throw gbxutilacfr::Exception( ERROR_INFO, "Number of fake latitudes and longitudes do not match" );

    stringstream ss;
    ss<<"Loaded "<<latitudes_.size()<<" fake points (lat,long) : ";
    for ( size_t i=0; i<latitudes_.size(); ++i )
        ss<<endl<<"("<<latitudes_[i]<<", "<<longitudes_[i]<<")";
    context_.tracer().info( ss.str() );
}

void
Driver::read( hydrointerfaces::Gps::Data& data )
{
    context_.tracer().debug( "Generating fake GPS data..." );
    hydroiceutil::now( data.timeStampSec, data.timeStampUsec );

    if ( neverSeeSatellites_ )
    {
        data.positionType = hydrointerfaces::Gps::GpsPositionTypeNotAvailable;
        data.satellites = 0;
    }
    else
    {
        if ( !longitudes_.empty() ) 
        {
            data.latitude = latitudes_[dataRecord_];
            data.longitude = longitudes_[dataRecord_];
            // every 20 seconds we switch to the next gps record
            // if at the end, start again
            if (numReads_ % 20 == 0)
            {
                if (dataRecord_==longitudes_.size()-1) {
                    dataRecord_ = 0;
                } else {
                    dataRecord_++;
                }
            }
        } else {
            data.latitude = -35.0;
            data.longitude = 150.0;
        }
        
        data.altitude = 50;
        double heading = data.heading + DEG2RAD(10);
        data.heading = heading; 
        data.speed = 0;
        data.climbRate = 0;
        data.satellites = 6;
        data.positionType = hydrointerfaces::Gps::GpsPositionTypeAutonomous;
        data.geoidalSeparation = 10;
    }
    
    numReads_++;
        
    IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
}

} // namespace

extern "C" {
    hydrointerfaces::GpsFactory *createDriverFactory()
    { return new gpsfake::Factory; }
}
