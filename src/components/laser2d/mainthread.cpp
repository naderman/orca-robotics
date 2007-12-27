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
    hydroutil::SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    context_(context)
{
    subStatus().setMaxHeartbeatInterval( 20.0 );

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
    stringstream exceptionSS;
    subStatus().setMaxHeartbeatInterval( 20.0 );

    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    // Dynamically load the library and find the factory
    std::string driverLibName = 
        orcaice::getPropertyWithDefault( prop, prefix+"DriverLib", "libOrcaLaser2dSickCarmen.so" );
    context_.tracer().debug( "MainThread: Loading driver library "+driverLibName, 4 );
    // The factory which creates the driver
    std::auto_ptr<hydrointerfaces::LaserScanner2dFactory> driverFactory;
    try {
        driverLib_.reset( new hydrodll::DynamicallyLoadedLibrary(driverLibName) );
        driverFactory.reset( 
            hydrodll::dynamicallyLoadClass<hydrointerfaces::LaserScanner2dFactory,DriverFactoryMakerFunc>
            ( *driverLib_, "createDriverFactory" ) );
    }
    catch (hydrodll::DynamicLoadException &e)
    {
        context_.tracer().error( e.what() );
        throw;
    }

    // create the driver
    while ( !isStopping() )
    {
        try {
            context_.tracer().info( "HwThread: Creating driver..." );
            driver_.reset( driverFactory->createDriver( config_, context_.toHydroContext() ) );
            // alexm: shouldn't this be 'break' ???
            return;
        }
        catch ( IceUtil::Exception &e ) {
            exceptionSS << "MainThread: Caught exception while creating driver: " << e;
        }
        catch ( std::exception &e ) {
            exceptionSS << "MainThread: Caught exception while initialising driver: " << e.what();
        }
        catch ( char *e ) {
            exceptionSS << "MainThread: Caught exception while initialising driver: " << e;
        }
        catch ( std::string &e ) {
            exceptionSS << "MainThread: Caught exception while initialising driver: " << e;
        }
        catch ( ... ) {
            exceptionSS << "MainThread: Caught unknown exception while initialising driver";
        }

        // we get here only after an exception was caught
        context_.tracer().error( exceptionSS.str() );
        subStatus().fault( exceptionSS.str() );          
        exceptionSS.clear();

        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));        
    }

    subStatus().setMaxHeartbeatInterval( 1.0 );
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
    stringstream exceptionSS;

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
                subStatus().warning( hydroLaserData_.warnings );
            }
            else
            {
                subStatus().ok();
            }

            stringstream ss;
            ss << "MainThread: Read laser data: " << orcaice::toString(orcaLaserData_);
            context_.tracer().debug( ss.str(), 5 );

            continue;

        } // end of try
        catch ( Ice::CommunicatorDestroyedException & ) {
            // This is OK: it means that the communicator shut down (eg via Ctrl-C)
            // somewhere in mainLoop. Eventually, component will tell us to stop.
        }
        catch ( const Ice::Exception &e ) {
            exceptionSS << "ERROR(mainthread.cpp): Caught unexpected exception: " << e;
        }
        catch ( const std::exception &e ) {
            exceptionSS << "ERROR(mainthread.cpp): Caught unexpected exception: " << e.what();
        }
        catch ( const std::string &e ) {
            exceptionSS << "ERROR(mainthread.cpp): Caught unexpected string: " << e;
        }
        catch ( const char *e ) {
            exceptionSS << "ERROR(mainthread.cpp): Caught unexpected char *: " << e;
        }
        catch ( ... ) {
            exceptionSS << "ERROR(mainthread.cpp): Caught unexpected unknown exception.";
        }

        if ( !exceptionSS.str().empty() ) {
            context_.tracer().error( exceptionSS.str() );
            subStatus().fault( exceptionSS.str() );     
            exceptionSS.clear();
            // Slow things down in case of persistent error
            sleep(1);
        }

        // If we got to here there's a problem.
        // Re-initialise the driver.
        initHardwareDriver();

    } // end of while

    // Laser hardware will be shut down in the driver's destructor.
}
