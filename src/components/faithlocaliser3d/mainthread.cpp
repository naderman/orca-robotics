/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaobj/localise3d.h>
#include <cmath>
#include "mainthread.h"

using namespace std;
using namespace faithlocaliser3d;

namespace {

    void odometryToLocalise( const orca::Odometry3dData &odData,
                             orca::Localise3dData       &loData,
                             const double               &varPosition,
                             const double               &varOrientation )
    {
        orca::Covariance3d c;
        c.xx = varPosition;
        c.xy = 0.0;
        c.xz = 0.0;
        c.xr = 0.0;
        c.xp = 0.0;
        c.xa = 0.0;
        c.yy = varPosition;
        c.yz = 0.0;
        c.yr = 0.0;
        c.yp = 0.0;
        c.ya = 0.0;
        c.zz = varPosition;
        c.zr = 0.0;
        c.zp = 0.0;
        c.za = 0.0;
        c.rr = varOrientation;
        c.rp = 0.0;
        c.ra = 0.0;
        c.pp = varOrientation;
        c.pa = 0.0;
        c.aa = varOrientation;

        loData.timeStamp = odData.timeStamp;
        loData.hypotheses.resize(1);
        loData.hypotheses[0].weight = 1.0;
        loData.hypotheses[0].mean   = odData.pose;
        loData.hypotheses[0].cov    = c;
    }
}

MainThread::MainThread( const orcaice::Context &context ) : 
    orcaice::SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    context_(context)
{
}

void 
MainThread::initialise()
{
    setMaxHeartbeatInterval( 10.0 );

    orcaice::activate( context_, this, subsysName() );
    // check for stop signal after retuning from multi-try
    if ( isStopping() )
        return;
    
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag()+".Config.";
    stdDevPosition_ = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"StdDevPosition", 0.05 );
    stdDevOrientation_ = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"StdDevOrientation", 1.0 );
    minInterPublishPeriodSec_ = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MinInterPublishPeriodSec", 1.0 ); 
    
    initNetworkInterface();
}

void
MainThread::work()
{
 
    // temp variables
    orca::Localise3dData localiseData;
    orca::Odometry3dData odomData;
    const double varPosition    = stdDevPosition_*stdDevPosition_;
    const double varOrientation = (stdDevOrientation_*M_PI/180.0)*(stdDevOrientation_*M_PI/180.0);

    const int timeoutMs = 1000;
    setMaxHeartbeatInterval( timeoutMs/1e3 );
    
    //
    // MAIN LOOP
    //
    orca::Time tLastPublish;
    tLastPublish.seconds=0;
    tLastPublish.useconds=0;

    while ( !isStopping() )
    {
        // this try makes this component robust to exceptions
        try
        {
            health().heartbeat();
            if ( odometry3dConsumer_->store().getNext( odomData, timeoutMs ) != 0 ) 
            {
                context_.tracer().debug( "Timeout waiting for data" );
                continue;
            }
    
            double timeSincePublish = orcaice::timeDiffAsDouble( orcaice::getNow(), tLastPublish );
            if ( timeSincePublish < minInterPublishPeriodSec_ )
                continue;
    
            odometryToLocalise( odomData, localiseData, varPosition, varOrientation );
            context_.tracer().debug( orcaobj::toString(localiseData), 5 );        
            localiseInterface_->localSetAndSend( localiseData );
            tLastPublish = orcaice::getNow();
        }
        catch ( ... ) 
        {
            orcaice::catchMainLoopExceptions( health() );
        }
    }
}

/////////////////////////////////

void
MainThread::initNetworkInterface()
{
    //
    // EXTERNAL REQUIRED INTERFACES
    //           
    odometry3dConsumer_ = new orcaifaceimpl::StoringOdometry3dConsumerImpl(context_);
    // multi-try function
    odometry3dConsumer_->subscribeWithTag( "Odometry3d", this, subsysName() );

    //
    // Get vehicleDescription
    //
    orca::VehicleDescription vehicleDescription;
    orcaice::getDescriptionWithTag<orca::Odometry3dPrx,orca::VehicleDescription>
        ( context_, "Odometry3d", vehicleDescription, this, subsysName() );
    
    // 
    // EXTERNAL PROVIDED INTERFACE
    //
    localiseInterface_ = new orcaifaceimpl::Localise3dImpl( vehicleDescription.geometry, "Localise3d", context_);
    // multi-try function
    localiseInterface_->initInterface( this, subsysName() );
}
