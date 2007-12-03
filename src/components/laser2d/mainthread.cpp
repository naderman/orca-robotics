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

namespace laser2d {

namespace {
    const char *SUBSYSTEM = "MainThread";
}

// MainThread::MainThread( orcaifaceimpl::LaserScanner2dImpl &laserInterface,
//                     const hydrointerfaces::LaserScanner2d::Config &config,
//                     hydrointerfaces::LaserScanner2dFactory &driverFactory,
//                     bool                                compensateRoll,
//                     const orcaice::Context             &context )
//     : laserInterface_(laserInterface),
//       config_(config),
//       driver_(0),
//       driverFactory_(driverFactory),
//       compensateRoll_(compensateRoll),
//       context_(context)
MainThread::MainThread( const orcaice::Context &context ) :
    laserInterface_(0),
    driver_(0),
    driverFactory_(0),
    driverLib_(0),
    context_(context)
{
    context_.status()->setMaxHeartbeatInterval( SUBSYSTEM, 20.0 );
    context_.status()->initialising( SUBSYSTEM );

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
        context_.tracer()->error( "Failed to validate laser configuration. "+config_.toString() );
        // this will kill this component
        throw hydroutil::Exception( ERROR_INFO, "Failed to validate laser configuration" );
    }

}

MainThread::~MainThread()
{
    if ( driver_ ) delete driver_;
}

void
MainThread::activate()
{
    while ( !isStopping() )
    {
        try {
            context_.activate();
            return;
        }
        catch ( hydroutil::Exception & e )
        {
            std::stringstream ss;
            ss << "MainThread::activate(): Caught exception: " << e.what();
            context_.tracer()->warning( ss.str() );
        }
        catch ( Ice::Exception & e )
        {
            std::stringstream ss;
            ss << "MainThread::activate(): Caught exception: " << e;
            context_.tracer()->warning( ss.str() );
        }
        catch ( ... )
        {
            context_.tracer()->warning( "MainThread::activate(): caught unknown exception." );
        }
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
        context_.status()->heartbeat( SUBSYSTEM );
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
//     bool compensateRoll;
    if ( NEAR(descr.offset.o.r,M_PI,0.001) && descr.offset.o.p==0.0 ) {
        // the offset is appropriate, now check the user preference (default is TRUE)
        compensateRoll_ = (bool)orcaice::getPropertyAsIntWithDefault( prop, prefix+"AllowRollCompensation", 1 );

        if ( compensateRoll_ ) {
            // now remove the roll angle, we'll compensate for it internally
            descr.offset.o.r = 0.0;
            context_.tracer()->info( "the driver will compensate for upside-down mounted sensor" );
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
    
    //
    // INIT INTERFACE
    //
    while ( !isStopping() )
    {
        try {
            laserInterface_->initInterface();
            context_.tracer()->debug( "Activated Laser interface" );
            return;
        }
        catch ( hydroutil::Exception &e )
        {
            context_.tracer()->warning( std::string("MainThread::initNetworkInterface(): ") + e.what() );
        }
        catch ( ... )
        {
            context_.tracer()->warning( "MainThread::initNetworkInterface(): caught unknown exception." );
        }
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
        context_.status()->heartbeat( SUBSYSTEM );
    }
}

void
MainThread::initHardwareDriver()
{
    context_.status()->setMaxHeartbeatInterval( SUBSYSTEM, 20.0 );

    // this function works for re-initialization as well
    if ( driver_ ) delete driver_;

    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    // Dynamically load the library and find the factory
    std::string driverLibName = 
        orcaice::getPropertyWithDefault( prop, prefix+"DriverLib", "libOrcaLaser2dSickCarmen.so" );
    context_.tracer()->debug( "MainThread: Loading driver library "+driverLibName, 4 );
    try {
        driverLib_ = new hydrodll::DynamicallyLoadedLibrary(driverLibName);
        driverFactory_ = 
            hydrodll::dynamicallyLoadClass<hydrointerfaces::LaserScanner2dFactory,DriverFactoryMakerFunc>
            ( *driverLib_, "createDriverFactory" );
    }
    catch (hydrodll::DynamicLoadException &e)
    {
        context_.tracer()->error( e.what() );
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
            context_.tracer()->info( "MainThread: Initialising driver..." );
            driver_ = driverFactory_->createDriver( config_, driverContext );
            return;
        }
        catch ( std::exception &e )
        {
            stringstream ss;
            ss << "MainThread: Caught exception while initialising driver: " << e.what();
            context_.tracer()->error( ss.str() );
            context_.status()->fault( SUBSYSTEM, ss.str() );
        }
        catch ( char *e )
        {
            stringstream ss;
            ss << "MainThread: Caught exception while initialising driver: " << e;
            context_.tracer()->error( ss.str() );
            context_.status()->fault( SUBSYSTEM, ss.str() );
        }
        catch ( std::string &e )
        {
            stringstream ss;
            ss << "MainThread: Caught exception while initialising driver: " << e;
            context_.tracer()->error( ss.str() );
            context_.status()->fault( SUBSYSTEM, ss.str() );
        }
        catch ( ... )
        {
            stringstream ss;
            ss << "MainThread: Caught unknown exception while initialising driver";
            context_.tracer()->error( ss.str() );
            context_.status()->fault( SUBSYSTEM, ss.str() );
        }
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));        
    }

    context_.status()->setMaxHeartbeatInterval( SUBSYSTEM, 1.0 );
}

void
MainThread::readData( orca::LaserScanner2dDataPtr &laserData )
{
    //
    // Read from the laser driver
    //            
    driver_->read( laserData->ranges, 
                   laserData->intensities,
                   laserData->timeStamp.seconds,
                   laserData->timeStamp.useconds );
    assert( (int)laserData->ranges.size()      == config_.numberOfSamples );
    assert( (int)laserData->intensities.size() == config_.numberOfSamples );

    // flip the scan left-to-right if we are configured to do so
    if ( compensateRoll_ ) {
        // NOTE: instead of copying around, we should be able to simply change the
        // start bearing and bearing increment.
        std::reverse( laserData->ranges.begin(), laserData->ranges.end() );
        std::reverse( laserData->intensities.begin(), laserData->intensities.end() );
    }
}

void
MainThread::walk()
{
    // Set up the laser-scan object
    orca::LaserScanner2dDataPtr laserData = new orca::LaserScanner2dData;
    laserData->minRange     = config_.minRange;
    laserData->maxRange     = config_.maxRange;
    laserData->fieldOfView  = config_.fieldOfView;
    laserData->startAngle   = config_.startAngle;
    laserData->ranges.resize( config_.numberOfSamples );
    laserData->intensities.resize( config_.numberOfSamples );

    // These functions catch their exceptions.
    activate();
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
            readData( laserData );
            laserInterface_->localSetAndSend( laserData );
            context_.status()->ok( SUBSYSTEM );

            stringstream ss;
            ss << "MainThread: Read laser data: " << orcaice::toString(laserData);
            context_.tracer()->debug( ss.str(), 5 );

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
            context_.tracer()->error( ss.str() );
            context_.status()->fault( SUBSYSTEM, ss.str() );
        }
        catch ( const std::exception &e )
        {
            std::stringstream ss;
            ss << "ERROR(mainloop.cpp): Caught unexpected exception: " << e.what();
            context_.tracer()->error( ss.str() );
            context_.status()->fault( SUBSYSTEM, ss.str() );
        }
        catch ( const std::string &e )
        {
            std::stringstream ss;
            ss << "ERROR(mainloop.cpp): Caught unexpected string: " << e;
            context_.tracer()->error( ss.str() );
            context_.status()->fault( SUBSYSTEM, ss.str() );            
        }
        catch ( const char *e )
        {
            std::stringstream ss;
            ss << "ERROR(mainloop.cpp): Caught unexpected char *: " << e;
            context_.tracer()->error( ss.str() );
            context_.status()->fault( SUBSYSTEM, ss.str() );
        }
        catch ( ... )
        {
            std::stringstream ss;
            ss << "ERROR(mainloop.cpp): Caught unexpected unknown exception.";
            context_.tracer()->error( ss.str() );
            context_.status()->fault( SUBSYSTEM, ss.str() );
        }

        // If we got to here there's a problem.
        // Re-initialise the driver.
        initHardwareDriver();

    } // end of while

    // Laser hardware will be shut down in the driver's destructor.
    context_.tracer()->debug( "dropping out from run()", 2 );
}

} // namespace
