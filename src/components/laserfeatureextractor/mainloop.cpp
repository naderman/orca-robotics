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
#include "laserconsumerI.h"
#include "mainloop.h"

// these drivers can always be built
#include "fakedriver.h"
#include "combineddriver.h"

using namespace std;
using namespace orca;
using namespace laserfeatures;

MainLoop::MainLoop( orcaifaceimpl::PolarFeature2dI &featureInterface,
                    const orcaice::Context &context )
    : driver_(NULL),
      featureInterface_(featureInterface),
//       laserDataProxy_(laserDataProxy),
//       polarFeaturesDataProxy_(polarFeaturesDataProxy),
      context_(context)
{
    sensorOffset_.p.x=0;
    sensorOffset_.p.y=0;
    sensorOffset_.p.z=0;
    sensorOffset_.o.r=0;
    sensorOffset_.o.p=0;
    sensorOffset_.o.y=0;
}

MainLoop::~MainLoop()
{
    if ( driver_ ) delete driver_;
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
        connectToLaser();
        getLaserDescription();

        context_.tracer()->debug( "loading 'combined' driver",3);
        driver_ = new CombinedDriver( context_, laserDescr_.maxRange, laserDescr_.numberOfSamples );
    }
    else
    {
        std::string errString = "Unknown algorithm driver: " + driverName;
        context_.tracer()->error( errString );
        exit(1);
    }
    
    context_.tracer()->debug("driver instantiated",5);
}

void 
MainLoop::connectToLaser()
{
    while ( isActive() )
    {
        try
        {
            context_.tracer()->debug( "Connecting to laser...", 2 );
            orcaice::connectToInterfaceWithTag<LaserScanner2dPrx>( context_, laserPrx_, "Laser" );
            context_.tracer()->debug("connected to a 'Laser' interface", 2 );
            break;
        }
        catch ( const std::exception &e )
        {
            stringstream ss;
            ss << "failed to connect to laser interface: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        catch ( const Ice::Exception &e )
        {
            stringstream ss;
            ss << "failed to connect to laser interface: " << e;
            context_.tracer()->error( ss.str() );
        }
        catch ( ... )
        {
            context_.tracer()->error( "Failed to connect to laser interface for unknown reason." );
        }
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
    }

    // create a callback object to recieve scans
    Ice::ObjectPtr consumer = new LaserConsumerI( laserDataProxy_ );
    orca::RangeScanner2dConsumerPrx callbackPrx =
        orcaice::createConsumerInterface<orca::RangeScanner2dConsumerPrx>( context_, consumer );

    while ( isActive() )
    {
        try
        {
            context_.tracer()->debug( "Subscribing to laser...", 2 );
            laserPrx_->subscribe( callbackPrx );
            break;
        }
        catch ( const std::exception &e )
        {
            stringstream ss;
            ss << "failed to subscribe to laser: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        catch ( const Ice::Exception &e )
        {
            stringstream ss;
            ss << "failed to subscribe to laser: " << e;
            context_.tracer()->error( ss.str() );
        }
        catch ( ... )
        {
            context_.tracer()->error( "Failed to subscribe to laser for unknown reason." );
        }
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
    }
}

void 
MainLoop::getLaserDescription()
{
    while ( isActive() )
    {
        try
        {
            context_.tracer()->debug( "Getting laser description...", 2 );
            laserDescr_ = laserPrx_->getDescription();
            stringstream ss;
            ss << "Got laser description: " << orcaice::toString( laserDescr_ );
            context_.tracer()->info( ss.str() );
            sensorOffset_ = laserDescr_.offset;
            if ( sensorOffsetOK( sensorOffset_ ) )
                return;
        }
        catch ( const std::exception &e )
        {
            stringstream ss;
            ss << "failed to retreive laser description: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        catch ( const Ice::Exception &e )
        {
            stringstream ss;
            ss << "failed to retreive laser description: " << e;
            context_.tracer()->error( ss.str() );
        }
        catch ( ... )
        {
            context_.tracer()->error( "Failed to retreive laser description for unknown reason." );
        }
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
    }
}

void
MainLoop::initInterface()
{
    while ( isActive() )
    {
        try {
            context_.tracer()->debug( "Initialising interface...", 2 );
            featureInterface_.initInterface();
            context_.tracer()->debug( "Activated PolarFeature2d interface" );
            return;
        }
        catch ( orcaice::Exception &e )
        {
            context_.tracer()->warning( std::string("MainLoop::establishInterface(): ") + e.what() );
        }
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
    }
}

void MainLoop::run()
{
    initInterface();
    initDriver();

    context_.tracer()->debug( "MainLoop: Entering major loop." );

    // Loop forever till we get shut down.
    while ( isActive() )
    {
        try
        {
            orca::PolarFeature2dDataPtr featureData = new orca::PolarFeature2dData;
            // don't need to create this one, it will be cloned from the proxy
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
                int ret = laserDataProxy_.getNext ( laserData, timeoutMs );
                if ( ret != 0 ) {
                    stringstream ss;
                    ss << "Timed out (" << timeoutMs << "ms) waiting for laser data.";
                    context_.tracer()->warning( ss.str() );
                    continue;
                }
                
                // cout << "INFO(algorithmhandler.cpp): Getting laserData of size "
                //      << laserData->ranges.size() << " from proxy" << endl << endl;

                //
                // execute algorithm to compute features
                //
                driver_->computeFeatures( laserData, featureData );

                // convert to the robot frame CS
                convertToRobotCS( featureData );

                // features have the same time stamp as the raw scan
                featureData->timeStamp = laserData->timeStamp;

                featureInterface_.localSetAndSend( featureData );

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
        catch ( const std::string &e )
        {
            std::stringstream ss;
            ss << "MainLoop: unexpected std::string exception: " << e;
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
convertPointToRobotCS( double &range,
                       double &bearing,
                       const CartesianPoint &offsetXyz,
                       const OrientationE offsetAngles )
{
    CartesianPoint LaserXy, RobotXy;
    
    LaserXy.x = cos(bearing) * range;
    LaserXy.y = sin(bearing) * range;
    RobotXy.x = LaserXy.x*cos(offsetAngles.y) - LaserXy.y*sin(offsetAngles.y) + offsetXyz.x;
    RobotXy.y = LaserXy.x*sin(offsetAngles.y) + LaserXy.y*cos(offsetAngles.y) + offsetXyz.y;
    range   = sqrt(RobotXy.x*RobotXy.x + RobotXy.y*RobotXy.y);
    bearing = atan2(RobotXy.y,RobotXy.x);
}

void 
MainLoop::convertToRobotCS( const PolarFeature2dDataPtr & featureData )
{
    CartesianPoint offsetXyz = sensorOffset_.p;
    OrientationE   offsetAngles = sensorOffset_.o;
    
    for (unsigned int i=0; i<featureData->features.size(); i++ )
    {
        // a bit ugly...
        orca::SinglePolarFeature2dPtr ftr = featureData->features[i];
        if ( ftr->ice_isA( "::orca::PointPolarFeature2d" ) )
        {
            orca::PointPolarFeature2d& f = dynamic_cast<orca::PointPolarFeature2d&>(*ftr);
            assert( f.p.r >= 0 && f.p.r < laserDescr_.maxRange );
            assert( f.p.o > laserDescr_.startAngle && 
                    f.p.o < laserDescr_.startAngle+laserDescr_.fieldOfView );
            assert( f.rangeSd >= 0 && f.bearingSd >= 0 );
            convertPointToRobotCS( f.p.r, f.p.o, offsetXyz, offsetAngles );
        }
        else if ( ftr->ice_isA( "::orca::LinePolarFeature2d" ) )
        {
            orca::LinePolarFeature2d& f = dynamic_cast<orca::LinePolarFeature2d&>(*ftr);
#ifndef NDEBUG
            // Check a few things...
            // (Allow slack because funny things can happen as endpoints are projected)
            const bool EPSR=0.5;
            const bool EPSB=5*M_PI/180.0;
            if ( f.startSighted )
            {
                assert( f.start.r >= -EPSR && f.start.r < laserDescr_.maxRange+EPSR );
                assert( f.start.o > laserDescr_.startAngle-EPSB && 
                        f.start.o < laserDescr_.startAngle+laserDescr_.fieldOfView+EPSB );
            }
            if ( f.endSighted )
            {
                assert( f.end.r >= -EPSR && f.end.r < laserDescr_.maxRange+EPSR );
                assert( f.end.o > laserDescr_.startAngle-EPSB && 
                        f.end.o < laserDescr_.startAngle+laserDescr_.fieldOfView+EPSB );
            }
            assert ( f.rhoSd >= 0 && f.alphaSd >= 0 );
#endif
            convertPointToRobotCS( f.start.r, f.start.o, offsetXyz, offsetAngles );
            convertPointToRobotCS( f.end.r,   f.end.o,   offsetXyz, offsetAngles );
        }
        else
        {
            stringstream ss; ss<<"ERROR(mainloop.cpp): Unknown feature type: " << ftr->type;
            throw ss.str();
        }
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
