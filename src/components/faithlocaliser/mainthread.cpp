/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h>

#include "mainthread.h"
#include <orcaifaceimpl/localise2dImpl.h>
#include <orcaifaceimpl/bufferedconsumers.h>

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
        loData.hypotheses[0].mean.p.x = odData.pose.p.x;
        loData.hypotheses[0].mean.p.y = odData.pose.p.y;
        loData.hypotheses[0].mean.o   = odData.pose.o;
        loData.hypotheses[0].cov.xx   = varPosition;
        loData.hypotheses[0].cov.yy   = varPosition;
        loData.hypotheses[0].cov.tt   = varHeading;
        loData.hypotheses[0].cov.xy   = 0.0;
        loData.hypotheses[0].cov.xt   = 0.0;
        loData.hypotheses[0].cov.yt   = 0.0;    
    }

}

MainThread::MainThread( const orcaice::Context &context ) : 
    SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    context_(context)
{
    subStatus().setMaxHeartbeatInterval( 10.0 );

    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag()+".Config.";
    stdDevPosition_ = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"StdDevPosition", 0.05 );
    stdDevHeading_ = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"StdDevHeading", 1.0 );
    minInterPublishPeriodSec_ = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MinInterPublishPeriodSec", 1.0 );
}

void
MainThread::walk()
{
    //
    // ENABLE NETWORK CONNECTIONS
    //
    // multi-try function
    orcaice::activate( context_, this, subsysName() );

    //
    // EXTERNAL REQUIRED INTERFACES
    //           
    orcaifaceimpl::BufferedOdometry2dConsumerImplPtr odometry2dConsumer =
        new orcaifaceimpl::BufferedOdometry2dConsumerImpl( 10, gbxiceutilacfr::BufferTypeCircular,context_);
    // multi-try function
    odometry2dConsumer->subscribeWithTag( "Odometry2d", this, subsysName() );

    //
    // Get vehicleDescription
    //
    orca::Odometry2dPrx odometryPrx;
    // multi-try function
    orcaice::connectToInterfaceWithTag<orca::Odometry2dPrx>( context_, odometryPrx, "Odometry2d", this );
 
    orca::VehicleDescription vehicleDescription;
    while ( !isStopping() )
    {
        try
        {
            vehicleDescription = odometryPrx->getDescription();
            break;
        }
        catch ( std::exception &e )
        {
            stringstream ss; ss << "Failed to get vehicle description from odometry interface: " << e.what() << " Will try again after 3 seconds.";
            context_.tracer().error( ss.str() );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
    }
    
    // 
    // EXTERNAL PROVIDED INTERFACE
    //
    orcaifaceimpl::Localise2dImplPtr localiseInterface =
        new orcaifaceimpl::Localise2dImpl( vehicleDescription.geometry, "Localise2d", context_);
    // multi-try function
    localiseInterface->initInterface( this, subsysName() );

    // temp variables
    orca::Localise2dData localiseData;
    orca::Odometry2dData odomData;
    const double varPosition = stdDevPosition_*stdDevPosition_;
    const double varHeading = (stdDevHeading_*M_PI/180.0)*(stdDevHeading_*M_PI/180.0);
    
    subStatus().ok( "Initialized" );
    const int timeoutMs = 1000;
    subStatus().setMaxHeartbeatInterval( 2*timeoutMs );

    //
    // MAIN LOOP
    //
    orca::Time tLastPublish;
    tLastPublish.seconds=0;
    tLastPublish.useconds=0;

    while ( !isStopping() )
    {
        subStatus().heartbeat();
        if ( odometry2dConsumer->buffer().getAndPopNext( odomData, timeoutMs ) != 0 ) {
            continue;
        }

        double timeSincePublish = orcaice::timeDiffAsDouble( orcaice::getNow(), tLastPublish );
        if ( timeSincePublish < minInterPublishPeriodSec_ )
            continue;

        odometryToLocalise( odomData, localiseData, varPosition, varHeading );
        context_.tracer().debug( orcaobj::toString(localiseData), 5 );        
        localiseInterface->localSetAndSend( localiseData );
        tLastPublish = orcaice::getNow();
    }

    // to be nice to the publisher of information, unsubscribe before quitting.
    odometry2dConsumer->unsubscribe();
}
