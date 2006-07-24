/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "component.h"

#include "algorithmhandler.h"

// interface implementations
#include "laserconsumerI.h"
#include "polarfeature2dI.h"

using namespace std;
using namespace orca;
using namespace laserfeatures;

Component::Component()
    : orcaice::Component( "LaserFeatureExtractor" ),
      algorithmHandler_(0)
{
}

Component::~Component()
{
    // do not delete algorithmHandler_!!! They derive from Ice::Thread and self-destruct.
}

// warning: this function returns after it's done, all variable that need to be permanent must
//          be declared as member variables.
void Component::start()
{
    //
    // PROVIDED: PolarFeatures
    //
    // find IceStorm publisher
    // NetworkException will kill it, that's what we want.
    IceStorm::TopicPrx topicPrx = orcaice::connectToTopicWithTag<PolarFeature2dConsumerPrx>
                                        ( context(), polarFeaturePublisher_, "PolarFeature2d" );
    // create servant for direct connections and tell adapter about it
    Ice::ObjectPtr polarFeatureObj = new PolarFeature2dI( polarFeaturesDataBuffer_, topicPrx );
    // two possible exceptions will kill it here, that's what we want
    orcaice::createInterfaceWithTag( context(), polarFeatureObj, "PolarFeature2d" );


    // REQUIRED : Laser
    orca::LaserPrx laserPrx;
    // TODO: this will not actually quit on ctrl-c
    while ( true ) // ( isActive() )
    {
        try
        {
            orcaice::connectToInterfaceWithTag<LaserPrx>( context(), laserPrx, "Laser" );
            context().tracer()->debug("connected to a 'Laser' interface",5);
            break;
        }
        // includes common ex's: ConnectionRefusedException, ConnectTimeoutException
        catch ( const Ice::LocalException & e )
        {
            context().tracer()->info("failed to connect to a remote interface. Will try again after 2 seconds.");
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
        }
    }

    // create a callback object to recieve scans
    Ice::ObjectPtr consumer = new LaserConsumerI( laserDataBuffer_ );
    orca::RangeScannerConsumerPrx callbackPrx =
        orcaice::createConsumerInterface<orca::RangeScannerConsumerPrx>( context(), consumer );
    
    //
    // ENABLE NETWORK CONNECTIONS
    //
    // this may throw, but may as well quit right then
    activate();

    //
    // Subscribe for data
    //
    // TODO: this will not actually quit on ctrl-c
    while ( true ) // ( isActive() )
    {
        try
        {
            laserPrx->subscribe( callbackPrx );
            break;
        }
        catch ( const orca::SubscriptionFailedException & e )
        {
            tracer()->error( "failed to subscribe for data updates. Will try again after 3 seconds." );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
    }
    
    // the constructor may throw, we'll let the application shut us down
    algorithmHandler_ = new AlgorithmHandler( polarFeaturePublisher_,
                                              laserPrx,
                                              laserDataBuffer_,
                                              polarFeaturesDataBuffer_,
                                              context() );
    algorithmHandler_->start();
    
    // the rest is handled by the application/service
}

void Component::stop()
{
    if ( algorithmHandler_ ) {
        IceUtil::ThreadControl algoControl = algorithmHandler_->getThreadControl();
        tracer()->debug("stopping algorithm handler", 5 );
        algorithmHandler_->stop();
        tracer()->debug("joining algorithm handler", 5 );
        algoControl.join();
    }
}
