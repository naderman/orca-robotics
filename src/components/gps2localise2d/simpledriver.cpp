/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include "simpledriver.h"
#include <iostream>
#include <orca/vehicledescription.h>
#include <orcaice/orcaice.h>
#include <orcaobj/bros1.h>
#include <hydrogpsutil/latlon2mga.h>
#include <orcanavutil/orcanavutil.h>

using namespace std;

namespace gps2localise2d {

SimpleDriver::SimpleDriver( const orca::GpsDescription     &gpsDescr, 
                            const orca::VehicleDescription &vehicleDescr,
                            const orcaice::Context         &context )
    : gpsDescr_(gpsDescr),
      context_(context)
{
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag();
    prefix += ".Config.";

    // Read the offset, with zero as default
    orca::Frame2d offset;
    offset.p.x = 0;
    offset.p.y = 0;
    offset.o   = 0;
    offset = orcaobj::getPropertyAsFrame2dWithDefault( prop, prefix+"Offset", offset );
    offset_ = orcanavutil::convert( offset );

    // Ensure that the antenna offset is simple
    if ( gpsDescr_.antennaOffset.o.r != 0 ||
         gpsDescr_.antennaOffset.o.p != 0 )
    {
        stringstream ss;
        ss << "Can't handle non-zero antenna roll/pitch.  Received: " << orcaobj::toString( gpsDescr_.antennaOffset );
        context_.tracer().error( ss.str() );
        exit(1);
    }
    antennaTransform_ = hydronavutil::Pose( -gpsDescr_.antennaOffset.p.x,
                                            -gpsDescr_.antennaOffset.p.y,
                                            -gpsDescr_.antennaOffset.o.y );

    // Initialise GpsHeuristics
    double maxSpeed = 0.0;
    if (vehicleDescr.control->type==orca::VehicleControlVelocityDifferential)
    {
        orca::VehicleControlVelocityDifferentialDescription *controlDesc =        
            dynamic_cast<orca::VehicleControlVelocityDifferentialDescription*>(&(*(vehicleDescr.control)));
        maxSpeed = controlDesc->maxForwardSpeed;
    }
    else if (vehicleDescr.control->type==orca::VehicleControlVelocityBicycle)
    {
        orca::VehicleControlVelocityBicycleDescription *controlDesc =
                dynamic_cast<orca::VehicleControlVelocityBicycleDescription*>(&(*(vehicleDescr.control)));
        maxSpeed = controlDesc->maxForwardSpeed;   
    }
    else
    {
        maxSpeed = 100.0;
        stringstream ss;
        ss << "VehicleDescription unknown to this driver. Will set maximum vehicle speed to " << maxSpeed << "m/s";       
        context_.tracer().warning( ss.str() );
    }
        
    gpsHeuristics_.reset( new GpsHeuristics( context, maxSpeed ) );
}

bool 
SimpleDriver::compute( const orca::GpsData  &gpsData,
                       orca::Localise2dData &localiseData )
{
//     cout<<"TRACE(simpledriver.cpp): " << __func__ << endl;

    if (!gpsHeuristics_->haveValidFix( gpsData.positionType ) )
        return false;
    
    if (!gpsHeuristics_->haveEnoughSatellites( gpsData.satellites ) )
        return false;

    // cout<<"TRACE(simpledriver.cpp): gpsData: " << orcaobj::toString(gpsData) << endl;

    double easting, northing;
    int zone;

    hydrogpsutil::LatLon2MGA( gpsData.latitude, 
                              gpsData.longitude,
                              northing,
                              easting,
                              zone );
    
    int ret = gpsHeuristics_->checkSpeedAndPosition( northing, 
                                                     easting, 
                                                     gpsData.speed,
                                                     gpsData.timeStamp );
    if (ret!=0) return false;

    hydronavutil::Pose poseInMapGridCS( easting, 
                                        northing, 
                                        M_PI/2.0 - gpsData.heading );

    // Get the pose of the antenna in gps2localise2d coordinates
    hydronavutil::Pose antennaPoseInGps2LCS;
    hydronavutil::subtractInitialOffset( poseInMapGridCS,
                                         offset_,
                                         antennaPoseInGps2LCS );

    cout<<"TRACE(simpledriver.cpp): poseInMapGridCS: " << poseInMapGridCS << endl;
    cout<<"TRACE(simpledriver.cpp): antennaPoseInGps2LCS: " << antennaPoseInGps2LCS << endl;

    // Get the pose of the platform (relative to the antenna)
    hydronavutil::Pose platformPoseInGps2LCS;
    hydronavutil::addPoseOffset( antennaPoseInGps2LCS,
                                 antennaTransform_,
                                 platformPoseInGps2LCS,
                                 true );

    // Set uncertainties
    const double linearSigma = gpsData.horizontalPositionError;
    const double linearVariance = linearSigma*linearSigma;
    
    //
    // convert
    //
    localiseData.timeStamp = gpsData.timeStamp;
    localiseData.hypotheses.resize(1);

    localiseData.hypotheses[0].mean.p.x = platformPoseInGps2LCS.x();
    localiseData.hypotheses[0].mean.p.y = platformPoseInGps2LCS.y();
    localiseData.hypotheses[0].mean.o   = platformPoseInGps2LCS.theta();

    localiseData.hypotheses[0].cov.xx = linearVariance;
    localiseData.hypotheses[0].cov.yy = linearVariance;
    localiseData.hypotheses[0].cov.xy = 0.0;

    localiseData.hypotheses[0].cov.xt = 0.0;
    localiseData.hypotheses[0].cov.yt = 0.0;
    localiseData.hypotheses[0].cov.tt = 2*M_PI;

    localiseData.hypotheses[0].weight = 1.0;

    return true;
}


}
