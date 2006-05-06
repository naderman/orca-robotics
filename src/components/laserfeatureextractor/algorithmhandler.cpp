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

#include "algorithmhandler.h"

// these drivers can always be built
#include "fakedriver.h"
#include "combineddriver.h"

using namespace std;
using namespace orca;
using namespace laserfeatures;
using orcaice::operator<<;

AlgorithmHandler::AlgorithmHandler( const PolarFeature2dConsumerPrx &polarFeaturesConsumer,
                                    LaserPrx laserPrx,
                                    orcaice::PtrBuffer<LaserDataPtr> &laserDataBuffer, 
                                    orcaice::PtrBuffer<PolarFeature2dDataPtr> &polarFeaturesDataBuffer,
                                    const orcaice::Context & context )
    : polarFeaturesConsumer_(polarFeaturesConsumer),
      laserPrx_(laserPrx),
      laserDataBuffer_(laserDataBuffer),
      polarFeaturesDataBuffer_(polarFeaturesDataBuffer),
      context_(context)
{
    // this is the last place we can throw exceptions from.
    // after this the thread will be launched
    init();
}

void AlgorithmHandler::init()
{
    std::string prefix = context_.tag() + ".Config.";
    Ice::PropertiesPtr prop = context_.properties();

    // pull out config parameters
    CombinedDriver::Config config;

    // Which algorithms to use
    config.extractReflectors                 =
        orcaice::getPropertyAsIntWithDefault(    prop, prefix+"ExtractReflectors", 1);
    config.extractForegroundPoints           =
        orcaice::getPropertyAsIntWithDefault(    prop, prefix+"ExtractForegroundPoints", 0);
    config.extractCorners                    =
        orcaice::getPropertyAsIntWithDefault(    prop, prefix+"ExtractCorners", 0);
    config.extractDoors                      =
        orcaice::getPropertyAsIntWithDefault(    prop, prefix+"ExtractDoors", 0);

    assert( config.extractCorners == 0 || config.extractCorners == 1 );

    // Reflector extraction params
    config.maxDeltaRangeWithinReflector      =
        orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Reflectors.MaxDeltaRangeWithinReflector", 0.3 );
    config.maxDeltaRangeNearReflector        =
        orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Reflectors.MaxDeltaRangeNearReflector", 0.5 );
    config.minReflectorBrightness            =
        orcaice::getPropertyAsIntWithDefault(    prop, prefix+"Reflectors.MinBrightness", 1);

    // Foreground extraction params
    config.minForegroundWidth                =
        orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"FGPoints.MinForegroundWidth", 0.1);
    config.maxForegroundWidth                =
        orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"FGPoints.MaxForegroundWidth", 0.5);
    config.minForegroundBackgroundSeparation =
        orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"FGPoints.MinForegroundBackgroundSeparation", 0.5);

    
    std::string driverName = orcaice::getPropertyWithDefault( prop,
            prefix+"Driver", "combined" );
    
    if ( driverName == "combined" )
    {
        context_.tracer()->debug( "loading 'combined' driver",3);
        driver_ = new CombinedDriver( config );
    }
    else if ( driverName == "fake" )
    {
        context_.tracer()->debug( "loading 'fake' driver",3);
        driver_ = new FakeDriver();
    }
    else
    {
        std::string errString = "Unknown algorithm driver: " + driverName;
        context_.tracer()->error( errString );
        context_.tracer()->info( "Valid driver values are {'combined', 'fake'}" );
        throw orcaice::Exception( ERROR_INFO, errString );
    }
    
    context_.tracer()->debug("driver instantiated",5);
}

AlgorithmHandler::~AlgorithmHandler()
{
    delete driver_;
}

void AlgorithmHandler::run()
{
    cout<<"TRACE(algorithmhandler.cpp): run()" << endl;

    // we are in a different thread now, catch all stray exceptions
    try
    {
    
    orca::PolarFeature2dDataPtr featureData = new orca::PolarFeature2dData;
    // don't need to create this one, it will be cloned from the buffer
    orca::LaserDataPtr laserData;
       
    // get laser config and geometry (only once)
    laserConfigPtr_ = laserPrx_->getConfig();
    driver_->setMaxRange( laserConfigPtr_->maxRange );
    laserGeometryPtr_ = laserPrx_->getGeometry();
    checkLaserGeometry( laserGeometryPtr_ );

    // wake up every now and then to check if we are supposed to stop
    const int timeoutMs = 1000;
    
    //
    // This is the main loop
    //
    while( isActive() )
    {
        context_.tracer()->debug( "=== Start of main loop ===", 5 );

        //
        // block on arrivale of laser data
        //
        int ret = laserDataBuffer_.getAndPopNext ( laserData, timeoutMs );
        
        if (ret == 0)
        {
            //cout << "INFO(mainloop.cpp): Getting laserData of size " << laserData->ranges.size() << " from buffer" << endl << endl;

            //
            // execute algorithm to compute features
            //
            driver_->computeFeatures( laserConfigPtr_, laserData, featureData );

            // convert to the robot frame CS
            convertToRobotCS( featureData );

            // features have the same time stamp as the raw scan
            featureData->timeStamp = laserData->timeStamp;
            
            try
            {
                // push it to IceStorm
                polarFeaturesConsumer_->setData( featureData );
                
                context_.tracer()->debug( "sending polar features to IceStorm", 3 );
                context_.tracer()->debug( orcaice::toString( featureData ), 3 );
            }
            catch ( Ice::ConnectionRefusedException & e )
            {
                context_.tracer()->warning( "failed to send polar features to IceStorm", 2 );
            }

            //
            // Stick it into buffer, so pullers can get it
            //
            //cout << "INFO(algorithmhandler.cpp): Featuremap: " << featureData << endl;
            polarFeaturesDataBuffer_.push( featureData );
        }
        else {
            stringstream ss;
            ss << "Timed out (" << timeoutMs << "ms) waiting for laser data.";
            context_.tracer()->warning( ss.str() );
        }

    } // while
    
    //
    // unexpected exceptions
    //
    } // try
    catch ( const orca::OrcaException & e )
    {
        context_.tracer()->print( e.what );
        context_.tracer()->error( "unexpected (remote?) orca exception.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const orcaice::Exception & e )
    {
        context_.tracer()->print( e.what() );
        context_.tracer()->error( "unexpected (local?) orcaice exception.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const Ice::Exception & e )
    {
        cout<<e<<endl;
        context_.tracer()->error( "unexpected Ice exception.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const std::exception & e )
    {
        // once caught this beast in here, don't know who threw it 'St9bad_alloc'
        cout<<e.what()<<endl;
        context_.tracer()->error( "unexpected std exception.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( ... )
    {
        context_.tracer()->error( "unexpected exception from somewhere.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }

    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
}

void AlgorithmHandler::convertToRobotCS( const PolarFeature2dDataPtr & featureData )
{
    CartesianPoint offsetXyz = laserGeometryPtr_->offset.p;
    OrientationE   offsetAngles = laserGeometryPtr_->offset.o;
    
    CartesianPoint LaserXy, RobotXy;
    PolarPoint2d polarPointRobot;
    
    for (uint i=0; i<featureData->features.size(); i++ )
    {
        LaserXy.x = cos(featureData->features[i]->p.o) * featureData->features[i]->p.r;
        LaserXy.y = sin(featureData->features[i]->p.o) * featureData->features[i]->p.r;
        RobotXy.x = LaserXy.x*cos(offsetAngles.y) - LaserXy.y*sin(offsetAngles.y) + offsetXyz.x;
        RobotXy.y = LaserXy.x*sin(offsetAngles.y) + LaserXy.y*cos(offsetAngles.y) + offsetXyz.y;
        polarPointRobot.r = sqrt(RobotXy.x*RobotXy.x + RobotXy.y*RobotXy.y);
        polarPointRobot.o = atan2(RobotXy.y,RobotXy.x);

        featureData->features[i]->p = polarPointRobot;
    }
}

void 
AlgorithmHandler::checkLaserGeometry( const orca::RangeScannerGeometryPtr geom )
{
    bool geomOK = true;
    if ( geom->offset.p.z != 0.0 )
    {
        stringstream ss;
        ss << "Can't handle non-zero 'z' component in laser offset.  geom was: " << geom;
        context_.tracer()->error( ss.str() );
        geomOK = false;
    }
    if ( geom->offset.o.r != 0.0 || geom->offset.o.p != 0.0 )
    {
        stringstream ss;
        ss << "Can't handle non-zero roll or pitch in laser offset.  geom was: " << geom;
        context_.tracer()->error( ss.str() );
        geomOK = false;
    }

    if ( !geomOK )
    {
        throw std::string( "Couldn't handle laser offset" );
    }
}
