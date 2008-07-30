/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include "simpledriver.h"
#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h>
#include <hydronavutil/offset.h>
#include <hydrogpsutil/latlon2mga.h>

using namespace std;

namespace gps2localise2d {

namespace {
    void transform( orca::Frame2d &p, const orca::Frame2d &f )
    {
        hydronavutil::transformPoint2d( p.p.x,
                                       p.p.y,
                                       f.p.x,
                                       f.p.y,
                                       f.o );
        p.o += f.o;
        hydronavutil::normaliseAngle( p.o );
    }
}

SimpleDriver::SimpleDriver( const orca::GpsDescription &descr,
                            const orcaice::Context &context )
    : descr_(descr),
      context_(context)
{
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag();
    prefix += ".Config.";

    // Read the offset, with zero as default
    offset_.p.x = 0;
    offset_.p.y = 0;
    offset_.o   = 0;
    offset_ = orcaobj::getPropertyAsFrame2dWithDefault( prop, prefix+"Offset", offset_ );

    // Ensure that the antenna offset is simple
    if ( descr_.antennaOffset.o.r != 0 ||
         descr_.antennaOffset.o.p != 0 )
    {
        stringstream ss;
        ss << "Can't handle non-zero antenna roll/pitch.  Received: " << orcaobj::toString( descr_.antennaOffset );
        context_.tracer().error( ss.str() );
        exit(1);
    }
    antennaTransform_.p.x = -descr_.antennaOffset.p.x;
    antennaTransform_.p.y = -descr_.antennaOffset.p.y;
    antennaTransform_.o   = -descr_.antennaOffset.o.y;
}

bool 
SimpleDriver::compute( const orca::GpsData  &gpsData,
                       orca::Localise2dData &localiseData )
{
    const int MIN_NUM_SATELLITES = 4;

    if ( gpsData.positionType == orca::GpsPositionTypeNotAvailable )
        return false;
    if ( gpsData.satellites < MIN_NUM_SATELLITES )
    {
        cout<<"TRACE(simpledriver.cpp): Warning: not enough satellites("<<gpsData.satellites<<") for reliable fix." << endl;
        return false;
    }

    // cout<<"TRACE(simpledriver.cpp): gpsData: " << orcaobj::toString(gpsData) << endl;

    double easting, northing;
    int zone;

    hydrogpsutil::LatLon2MGA( gpsData.latitude, 
                             gpsData.longitude,
                             northing,
                             easting,
                             zone );

    localiseData.timeStamp = gpsData.timeStamp;
    localiseData.hypotheses.resize(1);

    localiseData.hypotheses[0].mean.p.x = easting;
    localiseData.hypotheses[0].mean.p.y = northing;
    localiseData.hypotheses[0].mean.o   = M_PI/2.0-gpsData.heading; // M_PI/2.0*3.0 - gpsData.heading;

    const double linearSigma = gpsData.horizontalPositionError;
    const double linearVariance = linearSigma*linearSigma;

    localiseData.hypotheses[0].cov.xx = linearVariance;
    localiseData.hypotheses[0].cov.yy = linearVariance;
    localiseData.hypotheses[0].cov.xy = 0.0;

    localiseData.hypotheses[0].cov.xt = 0.0;
    localiseData.hypotheses[0].cov.yt = 0.0;
    localiseData.hypotheses[0].cov.tt = 2*M_PI;

    localiseData.hypotheses[0].weight = 1.0;

    // Get the pose of the antenna in global coordinates
    transform( localiseData.hypotheses[0].mean, offset_ );

    // Get the pose of the platform
    hydronavutil::addPoseOffset( localiseData.hypotheses[0].mean.p.x,
                                localiseData.hypotheses[0].mean.p.y,
                                localiseData.hypotheses[0].mean.o,
                                antennaTransform_.p.x,
                                antennaTransform_.p.y,
                                antennaTransform_.o,
                                true );

    return true;
}


}
