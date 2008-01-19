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
#include <hydrogpsutil/latlon2mga.h>

#include "mainthread.h"

#include "driver.h"
#include "fakegpsdriver.h"
#include "garmin/garmindriver.h"

using namespace std;
using namespace gps;

MainThread::MainThread( const orcaice::Context& context ) :
    SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    driver_(0),
    context_(context)
{
    subStatus().setMaxHeartbeatInterval( 10.0 );
}

MainThread::~MainThread()
{
    delete driver_;
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

    orcaice::setInit( descr_.antennaOffset );
    descr_.antennaOffset = orcaice::getPropertyAsFrame3dWithDefault( prop, prefix+"AntennaOffset", descr_.antennaOffset );

    stringstream ss;
    ss << "Loaded sensor description: " << orcaice::toString( descr_ );
    context_.tracer().debug( ss.str(), 2 );

    // create servant for direct connections
    gpsInterface_ = new orcaifaceimpl::GpsImpl( descr_, "Gps", context_ );

    // multi-try init
    gpsInterface_->initInterface( this, subsysName() );
}

void
MainThread::initHardwareDriver()
{
    subStatus().setMaxHeartbeatInterval( 10.0 );

    // this function works for re-initialization as well
    if ( driver_ ) delete driver_;

    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    //
    // Driver
    // 
    std::string driverName;
    int ret = orcaice::getProperty( prop, prefix+"Driver", driverName );
    if ( ret != 0 )
    {
        // unrecoverable error
        context_.shutdown(); 
        std::string errString = "Component: Couldn't determine gps type. Expected property '";
        errString += prefix + "Driver'";
        throw GpsException( errString );
    }

    if ( driverName == "garmin" )
    {
        std::string device;
        int ret = orcaice::getProperty( prop, prefix+"Device", device );
        if (ret!=0) {
            // unrecoverable error
            context_.shutdown(); 
            std::string errString = "Component: Couldn't determine serial port for garmin gps. Expected property '";
            errString += prefix + "Device'";
            throw GpsException( errString );
        }
        driver_ = new GarminGpsDriver( device.c_str(), context_ );
    }
    else if ( driverName == "fake" )
    {
        std::vector<double> latitudes, longitudes;
        for(unsigned int i=0; ; i++)
        {
            stringstream ss; ss << i;
            double latitude, longitude;
            if ( (orcaice::getPropertyAsDouble( prop, prefix+"FakeDriver.Latitude" + ss.str(), latitude )) ||
                 (orcaice::getPropertyAsDouble( prop, prefix+"FakeDriver.Longitude" + ss.str(), longitude )) )
            break;
            latitudes.push_back(latitude);
            longitudes.push_back(longitude);
        }
        driver_ = new FakeGpsDriver(latitudes,longitudes);
    }
    else
    {
        // unrecoverable error
        context_.shutdown(); 
        std::string errString = "Component: Unknown driver: " + driverName;
        context_.tracer().error( errString );
        throw GpsException( errString );
    }

    // initialize driver
    try {
        driver_->init();
    }
    catch( GpsException &e )
    {
        // unrecoverable error
        context_.shutdown(); 
        stringstream ss; ss << "Component: Failed to initialize GPS: " << e.what();
        context_.tracer().error( ss.str() );
        throw GpsException( ss.str() );
    }
}

void 
MainThread::reportBogusValues( orca::GpsData &gpsData )
{
    context_.tracer().debug("Reporting bogus values with positionType 0", 3);
    orcaice::setSane(gpsData);
    gpsData.positionType = orca::GpsPositionTypeNotAvailable;
    gpsInterface_->localSetAndSend(gpsData);
}

void
MainThread::walk()
{
    subStatus().setMaxHeartbeatInterval( 5.0 );  

    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    bool reportIfNoFix = orcaice::getPropertyAsIntWithDefault( prop, prefix+"ReportIfNoFix", 1 );

    // These functions catch their exceptions.
    orcaice::activate( context_, this, subsysName() );

    initNetworkInterface();
    initHardwareDriver();
  
    // temp data object
    orca::GpsData gpsData;

    //
    // Main loop
    //
    while ( !isStopping() )
    {
        context_.tracer().debug("Trying to read from driver.", 4);
        
        // keep trying to read until successful
        while ( !isStopping() )
        {
            try 
            {
                // Read from hardware: blocking call with timeout, drives the loop
                driver_->read( gpsData );
                break;
            }
            catch (GpsException &e)
            {
                stringstream ss;
                ss << "MainThread: Problem reading from GPS: " << e.what();
                context_.tracer().error( ss.str() );
                subStatus().fault( ss.str() );
            }

            //If the read threw then we should now try to re-initialise 
            try 
            {
                context_.tracer().debug("Trying to reinitialize now", 2);
                driver_->init();
            }
            catch (GpsException &e)
            {
                stringstream ss;
                ss << "MainThread: Problem reinitializing: " << e.what();
                context_.tracer().error( ss.str() );
            }
                
        }
        context_.tracer().debug("Read successfully from driver.", 5);


        if ( gpsData.positionType == orca::GpsPositionTypeNotAvailable )
        {
            context_.tracer().debug("No GPS fix", 2);
            if (reportIfNoFix) reportBogusValues(gpsData);
        }
        else
        {
            context_.tracer().debug("We have a GPS fix", 2);
                
            // Publish gpsData
            context_.tracer().debug("New GpsData: publishing now.", 5);
            context_.tracer().debug( orcaice::toString( gpsData ), 5 );
            gpsInterface_->localSetAndSend(gpsData);
        }        
            
        subStatus().ok();

    } // end of while

    // GPS hardware will be shut down in the driver's destructor.
    context_.tracer().debug( "dropping out from run()", 5 );
}
