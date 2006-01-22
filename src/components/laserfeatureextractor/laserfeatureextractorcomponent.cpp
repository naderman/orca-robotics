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

#include <orcaiceutil/connectutils.h>

#include "laserfeatureextractorcomponent.h"
#include "mainloop.h"
#include "featureextractorbase.h"
#include "fakeextractor.h"
#include "extractorone.h"

using namespace std;
using namespace orca;

LaserFeatureExtractorComponent::LaserFeatureExtractorComponent()
    : orcaiceutil::Component( "LaserFeatureExtractor" )
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
    std::string prefix = tag();
    prefix += ".Config.";
    prop_ = communicator()->getProperties();
   
    // =============== REQUIRED: Laser =======================
    laserConsumer_ = new LaserConsumerI( laserDataBuffer_ );
    //! @todo subscribe ourselves by directly calling subscribe() on the laser object. --alexm
    orcaiceutil::subscribeToIceStormTopicWithTag( current(), (Ice::ObjectPtr&) laserConsumer_, "Laser" );
    orcaiceutil::connectToInterfaceWithTag<LaserPrx>( current(), laserPrx_, "Laser" );
    
    // ============ PROVIDED: PolarFeatures ==================
    //IceStorm proxy
    IceStorm::TopicPrx topicPrx =
        orcaiceutil::connectToIceStormTopicWithTag<PolarFeature2dConsumerPrx>
                    ( current(),polarFeatureConsumer_, "PolarFeatures2d" );
    // create servant for direct connections and tell adapter about it
    polarFeature_ = new PolarFeature2dI( polarFeaturesDataBuffer_, topicPrx );
    orcaiceutil::createInterfaceWithTag( current(), polarFeature_, "PolarFeatures2d" );
    
    // =========== ENABLE NETWORK CONNECTIONS ======================
    cout << "TRACE(laserfeatureextractorcomponent.cpp): calling ACTIVATE" << endl;
    activate();
   
    // ================== ALGORITHMS ================================
    std::string algorithmType;
    int ret = orcaiceutil::getProperty( prop_, prefix+"AlgorithmType", algorithmType );
    if ( ret != 0 )
    {
        std::string errString = "Couldn't determine algorithmType.  Expected property";
        errString += prefix + "AlgorithmType";
        throw orcaiceutil::OrcaIceUtilException( ERROR_INFO, errString );
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
        throw orcaiceutil::OrcaIceUtilException( ERROR_INFO, errString );
        cout<<"TRACE(laserfeatureextractorcomponent.cpp): Unknown algorithmType: " << algorithmType << endl;
        return;
    }
    
    mainLoop_ = new MainLoop( algorithm_, polarFeatureConsumer_, laserPrx_, laserDataBuffer_, polarFeaturesDataBuffer_, &prop_, prefix );
    mainLoop_->start();
    
    // the rest is handled by the application/service
}

void LaserFeatureExtractorComponent::stop()
{
    // Tell the main loop to stop
    mainLoop_->stop();

    // inputLoop_ is blocked on user input
    // the only way for it to realize that we want to stop is to give it some keyboard input.
    cout<<"Quitting..."<<endl;

    // Then wait for it
    mainLoop_->getThreadControl().join();
}
