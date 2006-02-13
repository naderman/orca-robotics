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

#include "laserfeatureextractorcomponent.h"

#include "mainloop.h"
#include "featureextractorbase.h"
#include "fakeextractor.h"
#include "extractorone.h"

#include <orcaice/orcaice.h>

using namespace std;
using namespace orca;

LaserFeatureExtractorComponent::LaserFeatureExtractorComponent()
    : orcaice::Component( "LaserFeatureExtractor" )
{
}

LaserFeatureExtractorComponent::~LaserFeatureExtractorComponent()
{
}

// warning: this function returns after it's done, all variable that need to be permanet must
//          be declared as member variables.
void LaserFeatureExtractorComponent::start()
{
    // config file parameters: none at the moment
    std::string prefix = tag() + ".Config.";
    prop_ = communicator()->getProperties();

    //
    // PROVIDED: PolarFeatures
    //
    // find IceStorm publisher
    IceStorm::TopicPrx topicPrx =
        orcaice::connectToTopicWithTag<PolarFeature2dConsumerPrx>
                    ( context(),polarFeaturePublisher_, "PolarFeature2d" );
    // create servant for direct connections and tell adapter about it
    polarFeature_ = new PolarFeature2dI( polarFeaturesDataBuffer_, topicPrx );
    orcaice::createInterfaceWithTag( context(), polarFeature_, "PolarFeature2d" );

    //
    // REQUIRED: Laser
    //
    Ice::ObjectPtr consumer  = new LaserConsumerI( laserDataBuffer_ );
    laserCallbackPrx_ = orcaice::createConsumerInterface<orca::RangeScannerConsumerPrx>( context(),
                                                                                             consumer );

    // connect to remote Laser interface
    orcaice::connectToInterfaceWithTag<LaserPrx>( context(), laserPrx_, "Laser" );
    
    //
    // ENABLE NETWORK CONNECTIONS
    //
    activate();

    //
    // Subscribe for data
    //
    try
    {
        laserPrx_->subscribe( laserCallbackPrx_ );
    }
    catch ( const orca::SubscriptionFailedException & e )
    {
        tracer()->error( "failed to subscribe to laser data. Quitting..." );
        exit(1);
    }
   
    // ================== ALGORITHMS ================================
    std::string algorithmType;
    int ret = orcaice::getProperty( prop_, prefix+"AlgorithmType", algorithmType );
    if ( ret != 0 )
    {
        std::string errString = "Couldn't determine algorithmType. Expected property ";
        errString += prefix + "AlgorithmType";
        tracer()->error( errString );
        throw orcaice::Exception( ERROR_INFO, errString );
    }
    
    if ( algorithmType == "fake" )
    {
         algorithm_ = new FakeExtractor;
    }
    else if ( algorithmType == "extractorOne" )
    {
         algorithm_ = new ExtractorOne;
    }
    else
    {
        std::string errString = "Unknown algorithmType: " + algorithmType;
        tracer()->error( errString );
        throw orcaice::Exception( ERROR_INFO, errString );
        return;
    }
    
    mainLoop_ = new MainLoop( algorithm_, polarFeaturePublisher_, laserPrx_, laserDataBuffer_, polarFeaturesDataBuffer_, &prop_, prefix );
    mainLoop_->start();
    
    // the rest is handled by the application/service
}

void LaserFeatureExtractorComponent::stop()
{
    tracer()->debug( "component is stopping...",5 );
    
    // Tell the main loop to stop
    mainLoop_->stop();


    // Then wait for it
    mainLoop_->getThreadControl().join();
}
