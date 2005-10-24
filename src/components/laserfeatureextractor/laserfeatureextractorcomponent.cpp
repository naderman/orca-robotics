/*
 *  Orca-Components: Components for robotics.
 *  
 *  Copyright (C) 2004
 *  
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
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
    // do not delete inputLoop_!!! It derives from Ice::Thread and deletes itself.
}

// warning: this function returns after it's done, all variable that need to be permanet must
//          be declared as member variables.
void LaserFeatureExtractorComponent::start()
{

    // config file parameters: none at the moment
    std::string prefix = componentTag();
    prefix += ".Config.";
    Ice::PropertiesPtr prop = communicator()->getProperties();
//     bool startEnabled = orcaiceutil::getPropertyAsIntWithDefault( prop, prefix+"StartEnabled", true );
    
    // =============== REQUIRED: Laser =======================
    laserConsumer_ = new LaserConsumerI( laserDataBuffer_ );
    orcaiceutil::subscribeConsumerToTopicUsingCfg( communicator(), adapter(), componentTag(), (Ice::ObjectPtr&) laserConsumer_, "Laser" );
    
    // ============ PROVIDED: PolarFeatures ==================
    // create servant for direct connections and tell adapter about it
    polarFeature_ = new PolarFeature2dI( polarFeaturesDataBuffer_ );
    string polarFeatureId = orcaiceutil::getPortName( communicator(), componentTag(), "PolarFeatures" );
    adapter()->add( polarFeature_, Ice::stringToIdentity( polarFeatureId ) );
    // Find IceStorm ConsumerProxy to push out data
    orcaiceutil::getIceStormConsumerProxy<PolarFeature2dConsumerPrx>( communicator(), componentTag(), "PolarFeatures", polarFeatureConsumer_ );
    
    // =========== ACTIVATE ADAPTER ======================
    adapter()->activate();
    cout<<"*** INFO: Adapter is initialized and running..."<<endl;
   
    // ================== ALGORITHMS ================================
    std::string algorithmType;
    int ret = orcaiceutil::getProperty( prop, prefix+"AlgorithmType", algorithmType );
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
        cout<<"TRACE(laserfeatureextractorcomponent.cpp): Unknown algorithmTypee: " << algorithmType << endl;
        return;
    }
    
    mainLoop_ = new MainLoop( algorithm_, polarFeatureConsumer_, laserDataBuffer_, polarFeaturesDataBuffer_, &prop, prefix );
    mainLoop_->start();
    
    // the rest is handled by the application/service
}

void LaserFeatureExtractorComponent::stop()
{
    // Tell the main loop to stop
    mainLoop_->stop();

    // inputLoop_ is blocked on user input
    // the only way for it to realize that we want to stop is to give it some keyboard input.
    cout<<"Quitting... Press any key to continue."<<endl;

    // Then wait for it
    mainLoop_->getThreadControl().join();
}
