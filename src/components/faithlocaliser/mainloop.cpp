/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "mainloop.h"
#include "odometry2dconsumerI.h"

using namespace std;
using namespace faithlocaliser;

namespace {
    const char *SUBSYSTEM = "mainloop";

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

MainLoop::MainLoop( const orcaice::Context &context )
    : context_(context)
{
    context_.status()->setMaxHeartbeatInterval( SUBSYSTEM, 10.0 );
    context_.status()->initialising( SUBSYSTEM );

    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag()+".Config.";
    stdDevPosition_ = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"StdDevPosition", 0.05 );
    stdDevHeading_ = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"StdDevHeading", 1.0 );
}

MainLoop::~MainLoop()
{
}

void
MainLoop::initNetwork()
{
    //
    // ENABLE NETWORK CONNECTIONS
    //
    // multi-try function
    orcaice::activate( context_, this );

    //
    // EXTERNAL REQUIRED INTERFACES
    //            
    // multi-try function
    orcaice::connectToInterfaceWithTag<orca::Odometry2dPrx>( context_, odometryPrx_, "Odometry2d", this );
 
    // create a callback object to recieve scans
    Ice::ObjectPtr consumer = new Odometry2dConsumerI( odometryPipe_ );
    orca::Odometry2dConsumerPrx consumerPrx =
        orcaice::createConsumerInterface<orca::Odometry2dConsumerPrx>( context_, consumer );

    //
    // Subscribe for data
    //
    while ( isActive() )
    {
        try
        {
            odometryPrx_->subscribe( consumerPrx );
            break;
        }
        catch ( const orca::SubscriptionFailedException & e )
        {
            context_.tracer()->error( "failed to subscribe for data updates. Will try again after 3 seconds." );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
    }
    
    //
    // Get vehicleDescription
    //
    orca::VehicleDescription vehicleDescription;
    while ( isActive() )
    {
        try
        {
            vehicleDescription = odometryPrx_->getDescription();
            break;
        }
        catch ( std::exception &e )
        {
            stringstream ss; ss << "Failed to get description from odometry interface: " << e.what() << " Will try again after 3 seconds.";
            context_.tracer()->error( ss.str() );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
    }
    
    // 
    // EXTERNAL PROVIDED INTERFACE
    //
    localiseInterface_ = new orcaifaceimpl::Localise2dIface( vehicleDescription.geometry, "Localise2d", context_);
    localiseInterface_->initInterface();
}


void
MainLoop::run()
{
    initNetwork();

    orca::Localise2dData localiseData;
    orca::Odometry2dData odomData;
    double varPosition = stdDevPosition_*stdDevPosition_;
    double varHeading = (stdDevHeading_*M_PI/180.0)*(stdDevHeading_*M_PI/180.0);
    
    try 
    {
        while ( isActive() )
        {
            // Get odometry info, time out every so often to check if we are cancelled
            const int TIMEOUT_MS = 1000;
            if ( odometryPipe_.getAndPopNext( odomData, TIMEOUT_MS ) != 0 ) 
            {
                stringstream ss;
                ss << "MainLoop: received no odometry for " << TIMEOUT_MS << "ms";
                context_.tracer()->debug( ss.str(), 2 );
                continue;
            }

            odometryToLocalise( odomData, localiseData, varPosition, varHeading );
            context_.tracer()->debug( orcaice::toString(localiseData), 5 );
            
            localiseInterface_->localSetAndSend( localiseData );
        }
    }
    catch ( const Ice::Exception & e )
    {
        stringstream ss;
        ss << "MainLoop:: Caught exception: " << e;
        context_.tracer()->error( ss.str() );
        context_.status()->fault( SUBSYSTEM, ss.str() );
    }
    catch ( const std::exception & e )
    {
        stringstream ss;
        ss << "MainLoop: Caught exception: " << e.what();
        context_.tracer()->error( ss.str() );
        context_.status()->fault( SUBSYSTEM, ss.str() );
    }
    catch ( ... )
    {
        context_.tracer()->error( "MainLoop: caught unknown unexpected exception.");
        context_.status()->fault( SUBSYSTEM, "MainLoop: caught unknown unexpected exception.");
    }

    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
}
