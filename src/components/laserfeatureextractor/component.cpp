/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp,
 *               George Mathews, Stef Williams.
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "component.h"

#include "mainloop.h"

// interface implementations
#include "laserconsumerI.h"

using namespace std;
using namespace orca;
using namespace laserfeatures;

Component::Component()
    : orcaice::Component( "LaserFeatureExtractor" ),
      featureInterface_(0),
      mainLoop_(0)
{
}

Component::~Component()
{
    // do not delete mainLoop_!!! They derive from Ice::Thread and self-destruct.
}

// warning: this function returns after it's done, all variable that need to be permanent must
//          be declared as member variables.
void Component::start()
{
    context().tracer()->info( "Starting component..." );

//     //
//     // PROVIDED: PolarFeatures
//     //
//     // find IceStorm publisher
//     // NetworkException will kill it, that's what we want.
//     IceStorm::TopicPrx topicPrx = orcaice::connectToTopicWithTag<PolarFeature2dConsumerPrx>
//                                         ( context(), polarFeaturePublisher_, "PolarFeature2d" );
//     // create servant for direct connections and tell adapter about it
//     Ice::ObjectPtr polarFeatureObj = new PolarFeature2dI( polarFeaturesDataBuffer_, topicPrx );
//     // two possible exceptions will kill it here, that's what we want
//     orcaice::createInterfaceWithTag( context(), polarFeatureObj, "PolarFeature2d" );

//     // REQUIRED : Laser
//     orca::LaserScanner2dPrx laserPrx;
//     while (true) // ( isActive() )
//     {
//         try
//         {
//             orcaice::connectToInterfaceWithTag<LaserScanner2dPrx>( context(), laserPrx, "Laser" );
//             context().tracer()->debug("connected to a 'Laser' interface",5);
//             break;
//         }
//         // includes common ex's: ConnectionRefusedException, ConnectTimeoutException
//         catch ( const Ice::LocalException & e )
//         {
//             context().tracer()->info("failed to connect to a remote interface. Will try again after 2 seconds.");
//             IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
//         }
//     }

//     // create a callback object to recieve scans
//     Ice::ObjectPtr consumer = new LaserConsumerI( laserDataBuffer_ );
//     orca::RangeScanner2dConsumerPrx callbackPrx =
//         orcaice::createConsumerInterface<orca::RangeScanner2dConsumerPrx>( context(), consumer );
    
    //
    // Instantiate External Interface
    //
    featureInterface_ = new orcaifaceimpl::PolarFeature2dI( "PolarFeature2d", context() );

    //
    // ENABLE NETWORK CONNECTIONS
    //
    // this may throw, but may as well quit right then
    activate();

//     //
//     // Subscribe for data
//     //
//     while (true) // ( isActive() )
//     {
//         try
//         {
//             laserPrx->subscribe( callbackPrx );
//             break;
//         }
//         catch ( const orca::SubscriptionFailedException & e )
//         {
//             tracer()->error( "failed to subscribe for data updates. Will try again after 3 seconds." );
//             IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
//         }
//     }
    
    mainLoop_ = new MainLoop( *featureInterface_,
                              context() );

//                               polarFeaturePublisher_,
//                               laserPrx,
//                               laserDataBuffer_,
//                               polarFeaturesDataBuffer_,
//                               context() );

    mainLoop_->start();
}

void Component::stop()
{
    tracer()->info("stopping component...");
    if ( mainLoop_ ) {
        IceUtil::ThreadControl algoControl = mainLoop_->getThreadControl();
        tracer()->debug("stopping algorithm handler", 5 );
        mainLoop_->stop();
        tracer()->debug("joining algorithm handler", 5 );
        algoControl.join();
    }
}
