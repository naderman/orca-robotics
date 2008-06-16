/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Mathew Ridley, Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h>

#include "mainthread.h"

using namespace std;
using namespace gps;

namespace {

void convert( const hydrointerfaces::Gps::Data& hydro, orca::GpsData& orca )
{
    orca.timeStamp.seconds = hydro.timeStampSec;
    orca.timeStamp.useconds = hydro.timeStampUsec;
    orca.utcTime.hours = hydro.utcTimeHrs;
    orca.utcTime.minutes = hydro.utcTimeMin;
    orca.utcTime.seconds = hydro.utcTimeSec;

    orca.latitude = hydro.latitude;
    orca.longitude = hydro.longitude;
    orca.altitude = hydro.altitude;

    orca.horizontalPositionError = hydro.horizontalPositionError;
    orca.verticalPositionError = hydro.verticalPositionError;

    orca.heading = hydro.heading;
    orca.speed = hydro.speed;
    orca.climbRate = hydro.climbRate;

    orca.satellites = hydro.satellites;
    orca.observationCountOnL1 = hydro.observationCountOnL1;
    orca.observationCountOnL2 = hydro.observationCountOnL2;
    // relies on the fact that the enums in orca and hydro are identical!
    orca.positionType = (orca::GpsPositionType)hydro.positionType;
    orca.geoidalSeparation = hydro.geoidalSeparation;
}

}

////////////////////////////

MainThread::MainThread( const orcaice::Context& context ) :
    SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    context_(context)
{
//     subStatus().setMaxHeartbeatInterval( 10.0 );

    // Gps::Config object is currently empty, no properties to read.
}

void
MainThread::initNetworkInterface()
{
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    //
    // SENSOR DESCRIPTION   
    //
    descr_.timeStamp = orcaice::getNow();

    orcaobj::setInit( descr_.antennaOffset );
    descr_.antennaOffset = orcaobj::getPropertyAsFrame3dWithDefault( prop, prefix+"AntennaOffset", descr_.antennaOffset );

    stringstream ss;
    ss << "Loaded sensor description: " << orcaobj::toString( descr_ );
    context_.tracer().debug( ss.str(), 2 );

    // create servant for direct connections
    gpsInterface_ = new orcaifaceimpl::GpsImpl( descr_, "Gps", context_ );

    // multi-try init
    gpsInterface_->initInterface( this, subsysName() );
}

void
MainThread::initHardwareDriver()
{
    subStatus().setMaxHeartbeatInterval( 30.0 );

    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    // Dynamically load the library and find the factory
    std::string driverLibName = 
        orcaice::getPropertyWithDefault( prop, prefix+"DriverLib", "libHydroGpsGarmin.so" );
    context_.tracer().debug( "MainThread: Loading driver library "+driverLibName, 4 );
    // The factory which creates the driver
    std::auto_ptr<hydrointerfaces::GpsFactory> driverFactory;
    try {
        driverLib_.reset( new hydrodll::DynamicallyLoadedLibrary(driverLibName) );
        driverFactory.reset( 
            hydrodll::dynamicallyLoadClass<hydrointerfaces::GpsFactory,DriverFactoryMakerFunc>
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
        std::stringstream exceptionSS;
        try {
            context_.tracer().info( "HwThread: Creating driver..." );
            driver_.reset(0);
            driver_.reset( driverFactory->createDriver( config_, context_.toHydroContext() ) );
            break;
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

        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));        
    }

    subStatus().setMaxHeartbeatInterval( 10.0 );
}

void
MainThread::walk()
{
    subStatus().setMaxHeartbeatInterval( 60.0 );  

    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    bool publishWithoutFix = orcaice::getPropertyAsIntWithDefault( prop, prefix+"ReportIfNoFix", 1 );

    // These functions catch their exceptions.
    orcaice::activate( context_, this, subsysName() );

    initNetworkInterface();
    initHardwareDriver();
  
    // temp data object
    hydrointerfaces::Gps::Data hydroData;
    orca::GpsData orcaData;

    //
    // Main loop
    //
    while ( !isStopping() )
    {
        stringstream exceptionSS;
        try 
        {
            // this blocks until new data arrives
            driver_->read( hydroData );
            subStatus().heartbeat();
            
            // convert hydro->orca
            convert( hydroData, orcaData );

            if ( orcaData.positionType != orca::GpsPositionTypeNotAvailable || publishWithoutFix ) 
            {
                context_.tracer().debug( orcaobj::toString( orcaData ), 5 );
                gpsInterface_->localSetAndSend( orcaData );
            }
            else
            {
                context_.tracer().debug( "No GPS fix. Not publishing data", 6 );
            }
            subStatus().ok();

            continue;

        } // end of try
        catch ( Ice::CommunicatorDestroyedException & ) {
            // This is OK: it means that the communicator shut down (eg via Ctrl-C)
            // somewhere in mainLoop. Eventually, component will tell us to stop.
        }
        catch ( const Ice::Exception &e ) {
            exceptionSS << string(__FILE__) << " Caught unexpected exception: " << e;
        }
        catch ( const std::exception &e ) {
            exceptionSS << string(__FILE__) << " Caught unexpected exception: " << e.what();
        }
        catch ( const std::string &e ) {
            exceptionSS << string(__FILE__) << " Caught unexpected string: " << e;
        }
        catch ( const char *e ) {
            exceptionSS << string(__FILE__) << " Caught unexpected char *: " << e;
        }
        catch ( ... ) {
            exceptionSS << string(__FILE__) << " Caught unexpected unknown exception.";
        }

        if ( !exceptionSS.str().empty() ) {
            context_.tracer().error( exceptionSS.str() );
            subStatus().fault( exceptionSS.str() );     
            // Slow things down in case of persistent error
            sleep(1);
        }

        // If we got to here there's a problem.
        // Re-initialise the driver, unless we are quitting
        if ( !isStopping() ) {
            initHardwareDriver();
        }

    } // end of while

}
