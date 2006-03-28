/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
    while ( isActive() ) {
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
    while ( isActive() )
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
    tracer()->debug( "component is stopping...",5 );

    // make sure that the main loop was actually created
    if ( algorithmHandler_ ) 
    {
        // Tell the main loop to stop
        tracer()->debug("stopping component the MainLoop...",5);
        algorithmHandler_->stop();
    
        IceUtil::ThreadControl tc = algorithmHandler_->getThreadControl();
    
        // Then wait for it
        tc.join();
    }
}
