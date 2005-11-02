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
                    LaserPrx laserPrx,
                    PtrBuffer<LaserDataPtr> &laserDataBuffer, 
                    PtrBuffer<PolarFeature2dDataPtr> &polarFeaturesDataBuffer,
                    Ice::PropertiesPtr *prop,
                    string prefix )
    : algorithm_(algorithm),
      polarFeaturesConsumer_(polarFeaturesConsumer),
      laserPrx_(laserPrx),
      laserDataBuffer_(laserDataBuffer),
      polarFeaturesDataBuffer_(polarFeaturesDataBuffer),
      prop_(prop),
      prefix_(prefix)
{
    cout << "INFO(mainloop.cpp): Constructor" << endl;
}

MainLoop::~MainLoop()
{
}


void MainLoop::run()
{
    try
    {
        PolarFeature2dDataPtr featuresPtr = new PolarFeature2dData;
        LaserDataPtr laserDataPtr = new LaserData;
        
        // pull out config parameters
        ConfigParameters configParameters;
        
        configParameters.backgroundRangeGate = getPropertyAsDoubleWithDefault( *prop_, prefix_+"BackgroundRangeGate", 80.0 );
        configParameters.targetRangeGate = getPropertyAsDoubleWithDefault( *prop_, prefix_+"TargetRangeGate", 0.5 );
        configParameters.minReturnNumber =  getPropertyAsIntWithDefault( *prop_, prefix_+"MinReturnNumber", 1); 
        configParameters.minBrightness =  getPropertyAsIntWithDefault( *prop_, prefix_+"MinBrightness", 1); 
        configParameters.extractReflectors =  getPropertyAsIntWithDefault( *prop_, prefix_+"ExtractReflectors", 1); 
        configParameters.extractForegroundPoints =  getPropertyAsIntWithDefault( *prop_, prefix_+"ExtractForegroundPoints", 0); 
        configParameters.extractCorners =  getPropertyAsIntWithDefault( *prop_, prefix_+"ExtractCorners", 0); 
        configParameters.extractDoors =  getPropertyAsIntWithDefault( *prop_, prefix_+"ExtractDoors", 0);    
        configParameters.minForegroundWidth = getPropertyAsDoubleWithDefault( *prop_, prefix_+"MinForegroundWidth", 0.1);
        configParameters.maxForegroundWidth = getPropertyAsDoubleWithDefault( *prop_, prefix_+"MaxForegroundWidth", 0.5);
        configParameters.minForegroundBackgroundSeparation = getPropertyAsDoubleWithDefault( *prop_, prefix_+"MinForegroundBackgroundSeparation", 0.5);
        
        // initialize algorithm
        algorithm_ -> initialize( &configParameters );
        
        // get laser config and geometry (only once)
        laserConfigPtr_ = laserPrx_->getConfig();
        laserGeometryPtr_ = laserPrx_->getGeometry();
        
        while( isActive() )
        {
            // block on laser data
            laserDataBuffer_.getNext ( laserDataPtr );
            //cout << "INFO(mainloop.cpp): Getting laserData of size " << laserDataPtr->ranges.size() << " from buffer" << endl << endl;
            
            // execute algorithm to compute features
            algorithm_ -> computeFeatures( laserConfigPtr_, laserDataPtr, featuresPtr );
            
            // convert to the robot frame CS
            convertToRobotCS( featuresPtr );
            try {
                // push it to IceStorm
                polarFeaturesConsumer_->setData( featuresPtr );
                cout << "INFO(mainloop.cpp): Sending polarfeatures to IceStorm" << endl << endl; 
                cout << featuresPtr;
            }
            catch ( Ice::ConnectionRefusedException &e ) {
                cout<<"TRACE(mainloop.cpp): WARNING: Failed push to IceStorm." << endl;
            }
                
            // Stick it into buffer, so pullers can get it
            polarFeaturesDataBuffer_.push( featuresPtr );

            IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(50));
        }
        cout<<"TRACE(mainloop.cpp): Exitting from run()" << endl;
    }
    catch ( Ice::CommunicatorDestroyedException &e )
    {
        cout<<"TRACE(mainloop.cpp): Caught exception "<< e << endl;
    }
}

void MainLoop::convertToRobotCS(PolarFeature2dDataPtr featuresPtr)
{
    CartesianPoint offsetXyz = laserGeometryPtr_->offset.p;
    OrientationE  offsetAngles = laserGeometryPtr_->offset.o;
    
    CartesianPoint LaserXy, RobotXy;
    PolarPoint2d polarPointRobot;
    
    for (uint i=0; i<featuresPtr->features.size(); i++ )
    {
        LaserXy.x = cos(featuresPtr->features[i].o) * featuresPtr->features[i].r;
        LaserXy.y = sin(featuresPtr->features[i].o) * featuresPtr->features[i].r;
        RobotXy.x = LaserXy.x*cos(offsetAngles.y) - LaserXy.y*sin(offsetAngles.y) + offsetXyz.x;
        RobotXy.y = LaserXy.x*sin(offsetAngles.y) + LaserXy.y*cos(offsetAngles.y) + offsetXyz.y;
        polarPointRobot.r = sqrt(RobotXy.x*RobotXy.x + RobotXy.y*RobotXy.y);
        polarPointRobot.o = atan2(RobotXy.y,RobotXy.x);
        featuresPtr->features[i] = polarPointRobot;
    }
    const IceUtil::Time currentTime = IceUtil::Time::now();
    featuresPtr->timeStamp = iceTimeToOrcaTime (currentTime);
}
