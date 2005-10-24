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

#include "mainloop.h"
#include "configparameters.h"

#include <orcaiceutil/orcaiceutil.h>

using namespace std;
using namespace orca;
using namespace orcaiceutil;
using orcaiceutil::operator<<;


MainLoop::MainLoop( FeatureExtractorBase *algorithm,
                    const PolarFeature2dConsumerPrx &polarFeaturesConsumer,
                    PtrBuffer<LaserDataPtr> &laserDataBuffer, 
                    PtrBuffer<PolarFeature2dSequenceDataPtr> &polarFeaturesDataBuffer,
                    Ice::PropertiesPtr *prop,
                    string prefix )
    : algorithm_(algorithm),
      polarFeaturesConsumer_(polarFeaturesConsumer),
      laserDataBuffer_(laserDataBuffer),
      polarFeaturesDataBuffer_(polarFeaturesDataBuffer),
      prop_(prop),
      prefix_(prefix)
{
}

MainLoop::~MainLoop()
{
}


void MainLoop::run()
{
    try
    {
        PolarFeature2dSequenceDataPtr features = new PolarFeature2dSequenceData;
        LaserDataPtr laserData = new LaserData;
        
        // pull out config parameters
        ConfigParameters configParameters;
        configParameters.backgroundRangeGate = getPropertyAsDoubleWithDefault( *prop_, prefix_+"backgroundRangeGate", 80.0 );
        configParameters.targetRangeGate = getPropertyAsDoubleWithDefault( *prop_, prefix_+"targetRangeGate", 0.5 );
        configParameters.minReturnNumber =  getPropertyAsIntWithDefault( *prop_, prefix_+"minReturnNumber", 1); 
        configParameters.minBrightness =  getPropertyAsIntWithDefault( *prop_, prefix_+"minBrightness", 1); 
        
        //TODO: FIX THESE
        configParameters.extractReflectors =  true; 
        configParameters.extractForegroundPoints =  false; 
        configParameters.extractCorners =  false; 
        configParameters.extractDoors =  false; 
//         configParameters.extractReflectors =  getPropertyAsBoolWithDefault( *prop_, prefix_+"extractReflectors", true); 
//         configParameters.extractForegroundPoints =  getPropertyAsBoolWithDefault( *prop_, prefix_+"extractForegroundPoints", false); 
//         configParameters.extractCorners =  getPropertyAsBoolWithDefault( *prop_, prefix_+"extractCorners", false); 
//         configParameters.extractDoors =  getPropertyAsBoolWithDefault( *prop_, prefix_+"extractDoors", false); 
        
        configParameters.minForegroundWidth = getPropertyAsDoubleWithDefault( *prop_, prefix_+"minForegroundWidth", 0.1);
        configParameters.maxForegroundWidth = getPropertyAsDoubleWithDefault( *prop_, prefix_+"maxForegroundWidth", 0.5);
        configParameters.minForegroundBackgroundSeparation = getPropertyAsDoubleWithDefault( *prop_, prefix_+"minForegroundBackgroundSeparation", 0.5);
        
        // initialize algorithm
        algorithm_ -> initialize( &configParameters );
    
        while( isActive() )
        {
            // block on laser data
            laserDataBuffer_.getNext ( laserData );
            cout << "INFO(mainloop.cpp): LaserData: " << laserData << endl << endl;
//             algorithm_ -> computeFeatures


            try {
                // push it to IceStorm
                polarFeaturesConsumer_->consumeData( features );
            }
            catch ( Ice::ConnectionRefusedException &e ) {
                cout<<"TRACE(mainloop.cpp): WARNING: Failed push to IceStorm." << endl;
            }
            
            // Stick it into buffer, so pullers can get it
            polarFeaturesDataBuffer_.push( features );
        }
        cout<<"TRACE(mainloop.cpp): Exitting from run()" << endl;
    }
    catch ( Ice::CommunicatorDestroyedException &e )
    {
        cout<<"TRACE(mainloop.cpp): Caught exception "<< e << endl;
    }
}

