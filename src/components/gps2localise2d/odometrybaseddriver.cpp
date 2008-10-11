/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include <iostream>
#include <orcaice/orcaice.h>
#include <hydronavutil/offset.h>
#include <hydrogpsutil/latlon2mga.h>
#include <orcaice/orcaice.h>

#include "odometrybaseddriver.h"

using namespace std;

namespace gps2localise2d {

OdometryBasedDriver::OdometryBasedDriver( const orca::GpsDescription     &gpsDescr, 
                                          const orca::VehicleDescription &vehicleDescr,
                                          const orcaice::Context         &context )
    : simpleDriver_(gpsDescr,vehicleDescr,context),
      isSetup_(false),
      context_(context)
{
    odomConsumer_ = new orcaifaceimpl::StoringOdometry2dConsumerImpl(context);

    prevTime_.seconds  = 0;
    prevTime_.useconds = 0;

    // TODO: should make these configurable.
    minHeadingUncertainty_ = 10.0*M_PI/180.0;
    linSpeedThreshold_ = 0.75;
    linFactor_         = 30.0*M_PI/180.0;
    rotFactor_         = 180*M_PI/180.0;
}

OdometryBasedDriver::~OdometryBasedDriver()
{
}

bool
OdometryBasedDriver::setup()
{
    orca::Odometry2dPrx   odomPrx;
    
    try {
        orcaice::connectToInterfaceWithTag<orca::Odometry2dPrx>( context_, odomPrx, "Odometry2d" );
    }
    catch( std::exception &e )
    {
        stringstream ss; ss << "Error while connecting to odometry: " << e.what();
        context_.tracer().error( ss.str() );
        return false;
    }
    
    try {
        odomPrx->subscribe( odomConsumer_->consumerPrx() );
    }
    catch( std::exception &e )
    {
        stringstream ss; ss << "Error while subscribing to odometry: " << e.what();
        context_.tracer().error( ss.str() );
        return false;
    }
    context_.tracer().info( "Subscribed for odometry" );

    return true;
}

double 
OdometryBasedDriver::calcHeadingUncertainty( hydronavutil::Pose &delta,
                                             double dt )
{
    // Uncertainty is low if we're moving fast in a straight line.
    double linSpeed = dt*hypot(delta.x(),delta.y());
    double rotSpeed = dt*delta.theta();

    double rotVal = fabs(rotSpeed);

    if ( linSpeed > linSpeedThreshold_ )
        linSpeed = linSpeedThreshold_;

    double linVal = linSpeedThreshold_-(linSpeed);

    double uncertainty = 
        minHeadingUncertainty_  +
        rotFactor_ * rotVal     +
        linFactor_ * linVal;

//     cout<<"TRACE(odometrybaseddriver.cpp): -----------------" << endl;
//     cout<<"TRACE(odometrybaseddriver.cpp): linSpeed: " << linSpeed << endl;
//     cout<<"TRACE(odometrybaseddriver.cpp): rotSpeed: " << rotSpeed*180.0/M_PI << "deg/s" << endl;
//     cout<<"TRACE(odometrybaseddriver.cpp): rotFactor_*rotVal = (" << rotFactor_ << "*" << rotVal << ")="<<rotFactor_*rotVal * 180.0/M_PI << "deg" << endl;
//     cout<<"TRACE(odometrybaseddriver.cpp): linFactor_*linVal = (" << linFactor_ << "*" << linVal << ")="<<linFactor_*linVal * 180.0/M_PI << "deg" << endl;
//     cout<<"TRACE(odometrybaseddriver.cpp): uncertainty: " << uncertainty*180.0/M_PI << "deg" << endl;


    return uncertainty;
}


bool 
OdometryBasedDriver::compute( const orca::GpsData  &gpsData,
                              orca::Localise2dData &localiseData )
{
    if ( !isSetup_ )
    {
        isSetup_ = setup();
        return false;
    }

    if ( simpleDriver_.compute( gpsData, localiseData ) == false )
    {
        context_.tracer().debug( "OdometryBasedDriver: could not (reliably) convert GPS data to Localise2d data." );
        return false;
    }

    // Expand position uncertainty: the GPS unit is calculating it
    // based on the locations of satellites, but it's not taking into
    // account things like the proximity of metal structures.
    // This dodgy hack might help.
    localiseData.hypotheses[0].cov.xx *= 2.0;
    localiseData.hypotheses[0].cov.yy *= 2.0;

    if ( odomConsumer_->store().isEmpty() )
    {
        context_.tracer().warning( "OdometryBasedDriver: no odometry received yet." );
        return false;
    }

    double dt = orcaice::timeDiffAsDouble( gpsData.timeStamp, prevTime_ );
    if ( dt > 3.0 )
    {
        // Haven't received odometry for a while...
        // Don't screw with the heading for this one
    }
    else
    {
        orca::Odometry2dData odom;
        odomConsumer_->store().get( odom );
        hydronavutil::Pose delta = odometryDifferentiator_.calcDelta( odom.pose.p.x,
                                                                      odom.pose.p.y,
                                                                      odom.pose.o );

        assert( localiseData.hypotheses.size() == 1 );
        localiseData.hypotheses[0].cov.tt = calcHeadingUncertainty( delta, dt );
    }

    prevTime_ = gpsData.timeStamp;
    return true;
}


}
