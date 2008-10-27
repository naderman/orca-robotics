/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaifacestring/laserscanner2d.h>
#include <orcaobj/orcaobj.h> // for setInit()
#include "mainsubsystem.h"

using namespace std;
using namespace laser2d;

MainSubsystem::MainSubsystem( const orcaice::Context &context ) :
    orcaice::Subsystem( context, "MainSubsystem" )
{
}

void
MainSubsystem::initialise()
{
    subStatus().setMaxHeartbeatInterval( 20.0 );

    initSettings();

    initHardwareDriver();

    if ( isStopping() )
        return;

    initNetworkInterface();
}

void
MainSubsystem::work() 
{
    subStatus().setMaxHeartbeatInterval( 1.0 );

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
            ss << "MainSubsystem: Read laser data: " << ifacestring::toString(orcaLaserData_);
            context_.tracer().debug( ss.str(), 5 );
        }
        catch ( ... ) 
        {
            orcaice::catchMainLoopExceptions( subStatus() );

            // Re-initialise the driver, unless we are stopping
            if ( !isStopping() ) {
                initHardwareDriver();
            }
        }

    } // end of while
}

void
MainSubsystem::finalise() 
{
    // Laser hardware will be shut down in the driver's destructor.
}

////////////////////

void
MainSubsystem::initSettings()
{
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
        throw gbxutilacfr::Exception( ERROR_INFO, "Failed to validate laser configuration" );
    }

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
}

void
MainSubsystem::initNetworkInterface()
{
    // activate component's communication
    activate( context_, this, subsysName() );

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
    orcaobj::setInit( descr.offset );
    descr.offset = orcaobj::getPropertyAsFrame3dWithDefault( prop, prefix+"Offset", descr.offset );

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
    orcaobj::setInit( descr.size );
    descr.size = orcaobj::getPropertyAsSize3dWithDefault( prop, prefix+"Size", descr.size );

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
MainSubsystem::initHardwareDriver()
{
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    // Dynamically load the library and find the factory
    std::string driverLibName = 
        orcaice::getPropertyWithDefault( prop, prefix+"DriverLib", "libOrcaLaser2dSickCarmen.so" );
    context_.tracer().debug( "MainSubsystem: Loading driver library "+driverLibName, 4 );
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
        // unrecoverable error
        context_.shutdown(); 
        throw;
    }

    // create the driver
    while ( !isStopping() )
    {
        try {
            context_.tracer().info( "HwThread: Creating driver..." );
            driver_.reset(0);
            driver_.reset( driverFactory->createDriver( config_, context_.toHydroContext() ) );
            break;
        }
        catch ( ... ) {
            orcaice::catchExceptionsWithStatusAndSleep( "initialising hardware driver", subStatus() );
        }   
    }
}

void
MainSubsystem::readData()
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
