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
#include <orcaobj/localise2d.h>
#include <cmath>
#include "mainthread.h"

using namespace std;
using namespace faithlocaliser;

namespace {

    void odometryToLocalise( const orca::Odometry2dData &odData,
                            orca::Localise2dData        &loData,
                            const double          &varPosition,
                            const double          &varHeading )
    {
    
        loData.timeStamp = odData.timeStamp;
        loData.hypotheses.resize(1);
        loData.hypotheses[0].weight = 1.0;
        loData.hypotheses[0].mean   = odData.pose;
        loData.hypotheses[0].cov.xx   = varPosition;
        loData.hypotheses[0].cov.yy   = varPosition;
        loData.hypotheses[0].cov.tt   = varHeading;
        loData.hypotheses[0].cov.xy   = 0.0;
        loData.hypotheses[0].cov.xt   = 0.0;
        loData.hypotheses[0].cov.yt   = 0.0;    
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
    orca::Localise2dData localiseData;
    orca::Odometry2dData odomData;
    const double varPosition = stdDevPosition_*stdDevPosition_;
    const double varHeading = (stdDevOrientation_*M_PI/180.0)*(stdDevOrientation_*M_PI/180.0);

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
            if ( odometry2dConsumer_->store().getNext( odomData, timeoutMs ) != 0 ) {
                continue;
            }
    
            double timeSincePublish = orcaice::timeDiffAsDouble( orcaice::getNow(), tLastPublish );
            if ( timeSincePublish < minInterPublishPeriodSec_ )
                continue;
    
            odometryToLocalise( odomData, localiseData, varPosition, varHeading );
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
    odometry2dConsumer_ = new orcaifaceimpl::StoringOdometry2dConsumerImpl(context_);
    // multi-try function
    odometry2dConsumer_->subscribeWithTag( "Odometry2d", this, subsysName() );

    //
    // Get vehicleDescription
    //
    orca::VehicleDescription vehicleDescription;
    orcaice::getDescriptionWithTag<orca::Odometry2dPrx,orca::VehicleDescription>
        ( context_, "Odometry2d", vehicleDescription, this, subsysName() );
    
    // 
    // EXTERNAL PROVIDED INTERFACE
    //
    localiseInterface_ = new orcaifaceimpl::Localise2dImpl( vehicleDescription.geometry, "Localise2d", context_);
    // multi-try function
    localiseInterface_->initInterface( this, subsysName() );
}
