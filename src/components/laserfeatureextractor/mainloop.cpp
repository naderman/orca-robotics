/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp,
 *               George Mathews, Stef Williams.
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "mainloop.h"

// these drivers can always be built
#include "fakedriver.h"
#include "combineddriver.h"

using namespace std;
using namespace orca;
using namespace laserfeatures;

MainLoop::MainLoop( const PolarFeature2dConsumerPrx &polarFeaturesConsumer,
                                    LaserScanner2dPrx laserPrx,
                                    orcaice::PtrBuffer<LaserScanner2dDataPtr> &laserDataBuffer, 
                                    orcaice::PtrBuffer<PolarFeature2dDataPtr> &polarFeaturesDataBuffer,
                                    const orcaice::Context & context )
    : polarFeaturesConsumer_(polarFeaturesConsumer),
      laserPrx_(laserPrx),
      laserDataBuffer_(laserDataBuffer),
      polarFeaturesDataBuffer_(polarFeaturesDataBuffer),
      context_(context)
{
    sensorOffset_.p.x=0;
    sensorOffset_.p.y=0;
    sensorOffset_.p.z=0;
    sensorOffset_.o.r=0;
    sensorOffset_.o.p=0;
    sensorOffset_.o.y=0;
}

void MainLoop::initDriver()
{
    std::string prefix = context_.tag() + ".Config.";
    Ice::PropertiesPtr prop = context_.properties();
    std::string driverName = orcaice::getPropertyWithDefault( prop,
            prefix+"Driver", "combined" );
    
    if ( driverName == "fake" )
    {
        context_.tracer()->debug( "loading 'fake' driver",3);
        driver_ = new FakeDriver();
    }
    else if ( driverName == "combined" )
    {
        orca::RangeScanner2dDescriptionPtr descr;
        while ( isActive() )
        {
            try {
                // get laser description
                descr = laserPrx_->getDescription();
                sensorOffset_ = descr->offset;
                if ( sensorOffsetOK( sensorOffset_ ) )
                    break;
                IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
            }
            catch ( Ice::Exception &e )
            {
                stringstream ss;
                ss << "MainLoop::initDriver: problem getting laser description: " << e;
                context_.tracer()->warning( ss.str() );
            }
        }

        context_.tracer()->debug( "loading 'combined' driver",3);
        driver_ = new CombinedDriver( context_, descr->maxRange, descr->numberOfSamples );
    }
    else
    {
        std::string errString = "Unknown algorithm driver: " + driverName;
        context_.tracer()->error( errString );
        exit(1);
    }
    
    context_.tracer()->debug("driver instantiated",5);
}

MainLoop::~MainLoop()
{
    delete driver_;
}

void MainLoop::run()
{
    initDriver();

    // Loop forever till we get shut down.
    while ( isActive() )
    {
        try
        {
            orca::PolarFeature2dDataPtr featureData = new orca::PolarFeature2dData;
            // don't need to create this one, it will be cloned from the buffer
            orca::LaserScanner2dDataPtr laserData;
       
            // wake up every now and then to check if we are supposed to stop
            const int timeoutMs = 1000;
    
            //
            // This is the main loop
            //
            while( isActive() )
            {
                context_.tracer()->debug( "=== Start of main loop ===", 5 );
                
                //
                // block on arrival of laser data
                //
                int ret = laserDataBuffer_.getAndPopNext ( laserData, timeoutMs );
                if ( ret != 0 ) {
                    stringstream ss;
                    ss << "Timed out (" << timeoutMs << "ms) waiting for laser data.";
                    context_.tracer()->warning( ss.str() );
                    continue;
                }
                
                // cout << "INFO(algorithmhandler.cpp): Getting laserData of size "
                //      << laserData->ranges.size() << " from buffer" << endl << endl;

                //
                // execute algorithm to compute features
                //
                driver_->computeFeatures( laserData, featureData );

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

            } // while

        } // try
        catch ( const orca::OrcaException & e )
        {
            std::stringstream ss;
            ss << "MainLoop: unexpected orca exception: " << e << ": " << e.what;
            context_.tracer()->error( ss.str() );
        }
        catch ( const Ice::Exception & e )
        {
            std::stringstream ss;
            ss << "MainLoop: unexpected Ice exception: " << e;
            context_.tracer()->error( ss.str() );
        }
        catch ( const std::exception & e )
        {
            std::stringstream ss;
            ss << "MainLoop: unexpected std exception: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        catch ( ... )
        {
            context_.tracer()->error( "MainLoop: unexpected exception from somewhere.");
        }
    }

    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
}

void 
MainLoop::convertToRobotCS( const PolarFeature2dDataPtr & featureData )
{
    CartesianPoint offsetXyz = sensorOffset_.p;
    OrientationE   offsetAngles = sensorOffset_.o;
    
    CartesianPoint LaserXy, RobotXy;
    PolarPoint2d polarPointRobot;
    
    for (unsigned int i=0; i<featureData->features.size(); i++ )
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

bool
MainLoop::sensorOffsetOK( const orca::Frame3d & offset )
{
    bool offsetOk = true;
    if ( offset.p.z != 0.0 )
    {
        stringstream ss;
        ss << "Can't handle non-zero 'z' component in laser offset. Offset: " << orcaice::toString(offset);
        context_.tracer()->error( ss.str() );
        offsetOk = false;
    }
    if ( offset.o.r != 0.0 || offset.o.p != 0.0 )
    {
        stringstream ss;
        ss << "Can't handle non-zero roll or pitch in laser offset. Offset: " << orcaice::toString(offset);
        context_.tracer()->error( ss.str() );
        offsetOk = false;
    }

    return offsetOk;
}
