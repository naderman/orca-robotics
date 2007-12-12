/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include "mainthread.h"

using namespace std;
using namespace laser2d;

MainThread::MainThread( const orcaice::Context &context ) :
    hydroutil::SafeThread( context.tracer(), context.status(), "MainThread" ),
    laserInterface_(0),
    driver_(0),
    driverFactory_(0),
    driverLib_(0),
    context_(context)
{
    context_.status().setMaxHeartbeatInterval( subsysName(), 20.0 );
    context_.status().initialising( subsysName() );

    //
    // Read settings
    //
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    config_.minRange = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MinRange", 0.0 );
    config_.maxRange = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MaxRange", 80.0 );

    config_.fieldOfView = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"FieldOfView", 180.0 )*DEG2RAD_RATIO;
    config_.startAngle = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"StartAngle", -RAD2DEG(config_.fieldOfView)/2.0 )*DEG2RAD_RATIO;

    config_.numberOfSamples = orcaice::getPropertyAsIntWithDefault( prop, prefix+"NumberOfSamples", 181 );

    if ( !config_.validate() ) {
        context_.tracer().error( "Failed to validate laser configuration. "+config_.toString() );
        // this will kill this component
        throw hydroutil::Exception( ERROR_INFO, "Failed to validate laser configuration" );
    }

}

MainThread::~MainThread()
{
    delete driver_;
    delete driverFactory_;
    delete driverLib_;
}

void
MainThread::initNetworkInterface()
{
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    //
    // SENSOR DESCRIPTION
    //
    orca::RangeScanner2dDescription descr;
    descr.timeStamp = orcaice::getNow();

    // transfer internal sensor configs
    descr.minRange        = config_.minRange;
    descr.maxRange        = config_.maxRange;
    descr.fieldOfView     = config_.fieldOfView;
    descr.startAngle      = config_.startAngle;
    descr.numberOfSamples = config_.numberOfSamples;

    // offset from the robot coordinate system
    orcaice::setInit( descr.offset );
    descr.offset = orcaice::getPropertyAsFrame3dWithDefault( prop, prefix+"Offset", descr.offset );

    // consider the special case of the sensor mounted level (pitch=0) but upside-down (roll=180)
    if ( NEAR(descr.offset.o.r,M_PI,0.001) && descr.offset.o.p==0.0 ) {
        // the offset is appropriate, now check the user preference (default is TRUE)
        compensateRoll_ = (bool)orcaice::getPropertyAsIntWithDefault( prop, prefix+"AllowRollCompensation", 1 );

        if ( compensateRoll_ ) {
            // now remove the roll angle, we'll compensate for it internally
            descr.offset.o.r = 0.0;
            context_.tracer().info( "the driver will compensate for upside-down mounted sensor" );
        }
    }
    else {
        // no need to consider it, the offset is inappropriate for roll compensation
        compensateRoll_ = false;
    }

    // size info should really be stored in the driver
    orcaice::setInit( descr.size );
    descr.size = orcaice::getPropertyAsSize3dWithDefault( prop, prefix+"Size", descr.size );

    //
    // EXTERNAL PROVIDED INTERFACE
    //

    laserInterface_ = new orcaifaceimpl::LaserScanner2dImpl( descr,
                                                              "LaserScanner2d",
                                                              context_ );
    // init
    laserInterface_->initInterface( this, subsysName() );
}

void
MainThread::initHardwareDriver()
{
    context_.status().setMaxHeartbeatInterval( subsysName(), 20.0 );

    // this function works for re-initialization as well
    if ( driver_ ) delete driver_;

    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    // Dynamically load the library and find the factory
    std::string driverLibName = 
        orcaice::getPropertyWithDefault( prop, prefix+"DriverLib", "libOrcaLaser2dSickCarmen.so" );
    context_.tracer().debug( "MainThread: Loading driver library "+driverLibName, 4 );
    try {
        driverLib_ = new hydrodll::DynamicallyLoadedLibrary(driverLibName);
        driverFactory_ = 
            hydrodll::dynamicallyLoadClass<hydrointerfaces::LaserScanner2dFactory,DriverFactoryMakerFunc>
            ( *driverLib_, "createDriverFactory" );
    }
    catch (hydrodll::DynamicLoadException &e)
    {
        context_.tracer().error( e.what() );
        throw;
    }

    // create the driver
    while ( !isStopping() )
    {
        string configPrefix = context_.tag()+".Config.";
        hydroutil::Properties props(
            context_.properties()->getPropertiesForPrefix(configPrefix), configPrefix );
        hydrointerfaces::Context driverContext( props, context_.tracer(), context_.status() );
        try {
            context_.tracer().info( "MainThread: Initialising driver..." );
            driver_ = driverFactory_->createDriver( config_, driverContext );
            return;
        }
        catch ( IceUtil::Exception &e )
        {
            stringstream ss;
            ss << "MainThread: Caught exception while initialising driver: " << e;
            context_.tracer().error( ss.str() );
            context_.status().fault( subsysName(), ss.str() );
        }
        catch ( std::exception &e )
        {
            stringstream ss;
            ss << "MainThread: Caught exception while initialising driver: " << e.what();
            context_.tracer().error( ss.str() );
            context_.status().fault( subsysName(), ss.str() );
        }
        catch ( char *e )
        {
            stringstream ss;
            ss << "MainThread: Caught exception while initialising driver: " << e;
            context_.tracer().error( ss.str() );
            context_.status().fault( subsysName(), ss.str() );
        }
        catch ( std::string &e )
        {
            stringstream ss;
            ss << "MainThread: Caught exception while initialising driver: " << e;
            context_.tracer().error( ss.str() );
            context_.status().fault( subsysName(), ss.str() );
        }
        catch ( ... )
        {
            stringstream ss;
            ss << "MainThread: Caught unknown exception while initialising driver";
            context_.tracer().error( ss.str() );
            context_.status().fault( subsysName(), ss.str() );
        }
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));        
    }

    context_.status().setMaxHeartbeatInterval( subsysName(), 1.0 );
}

void
MainThread::readData()
{
    //
    // Read from the laser driver
    //
    hydroLaserData_.haveWarnings = false;
    driver_->read( hydroLaserData_ );

    orcaLaserData_->timeStamp.seconds  = hydroLaserData_.timeStampSec;
    orcaLaserData_->timeStamp.useconds = hydroLaserData_.timeStampUsec;

    // flip the scan left-to-right if we are configured to do so
    if ( compensateRoll_ ) {
        // NOTE: instead of copying around, we should be able to simply change the
        // start bearing and bearing increment.
        std::reverse( orcaLaserData_->ranges.begin(), orcaLaserData_->ranges.end() );
        std::reverse( orcaLaserData_->intensities.begin(), orcaLaserData_->intensities.end() );
    }
}

void
MainThread::walk()
{
    // Set up the laser-scan objects
    orcaLaserData_ = new orca::LaserScanner2dData;
    orcaLaserData_->minRange     = config_.minRange;
    orcaLaserData_->maxRange     = config_.maxRange;
    orcaLaserData_->fieldOfView  = config_.fieldOfView;
    orcaLaserData_->startAngle   = config_.startAngle;
    orcaLaserData_->ranges.resize( config_.numberOfSamples );
    orcaLaserData_->intensities.resize( config_.numberOfSamples );

    // Point the pointers in hydroLaserData_ at orcaLaserData_
    hydroLaserData_.ranges      = &(orcaLaserData_->ranges[0]);
    hydroLaserData_.intensities = &(orcaLaserData_->intensities[0]);

    // These functions catch their exceptions.
    activate( context_, this, subsysName() );

    initNetworkInterface();
    initHardwareDriver();

    //
    // IMPORTANT: Have to keep this loop rolling, because the '!isStopping()' call checks for requests to shut down.
    //            So we have to avoid getting stuck anywhere within this main loop.
    //
    while ( !isStopping() )
    {
        try 
        {
            // this blocks until new data arrives
            readData();
            
            laserInterface_->localSetAndSend( orcaLaserData_ );
            if ( hydroLaserData_.haveWarnings )
            {
                context_.status().warning( subsysName(), hydroLaserData_.warnings );
            }
            else
            {
                context_.status().ok( subsysName() );
            }

            stringstream ss;
            ss << "MainThread: Read laser data: " << orcaice::toString(orcaLaserData_);
            context_.tracer().debug( ss.str(), 5 );

            continue;

        } // end of try
        catch ( Ice::CommunicatorDestroyedException & )
        {
            // This is OK: it means that the communicator shut down (eg via Ctrl-C)
            // somewhere in mainLoop. Eventually, component will tell us to stop.
        }
        catch ( const Ice::Exception &e )
        {
            std::stringstream ss;
            ss << "ERROR(mainloop.cpp): Caught unexpected exception: " << e;
            context_.tracer().error( ss.str() );
            context_.status().fault( subsysName(), ss.str() );
        }
        catch ( const std::exception &e )
        {
            std::stringstream ss;
            ss << "ERROR(mainloop.cpp): Caught unexpected exception: " << e.what();
            context_.tracer().error( ss.str() );
            context_.status().fault( subsysName(), ss.str() );
        }
        catch ( const std::string &e )
        {
            std::stringstream ss;
            ss << "ERROR(mainloop.cpp): Caught unexpected string: " << e;
            context_.tracer().error( ss.str() );
            context_.status().fault( subsysName(), ss.str() );            
        }
        catch ( const char *e )
        {
            std::stringstream ss;
            ss << "ERROR(mainloop.cpp): Caught unexpected char *: " << e;
            context_.tracer().error( ss.str() );
            context_.status().fault( subsysName(), ss.str() );
        }
        catch ( ... )
        {
            std::stringstream ss;
            ss << "ERROR(mainloop.cpp): Caught unexpected unknown exception.";
            context_.tracer().error( ss.str() );
            context_.status().fault( subsysName(), ss.str() );
        }

        // If we got to here there's a problem.
        // Re-initialise the driver.
        initHardwareDriver();

    } // end of while

    // Laser hardware will be shut down in the driver's destructor.
    context_.tracer().debug( "dropping out from run()", 2 );
}
