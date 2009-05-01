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
#include <orcaifaceutil/rangerarray.h>
#include <orcabros1/convertutils.h>
#include <orcaobj/orcaobj.h> // for getPropertyAs...()
#include <orca/bros1.h>
#include "mainthread.h"

using namespace std;
using namespace rangerarray;

MainThread::MainThread( const orcaice::Context &context ) :
    orcaice::SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    context_(context)
{
}

void
MainThread::initialise()
{
    setMaxHeartbeatInterval( 20.0 );

    initHardwareDriver();

    if ( isStopping() )
        return;

    initNetworkInterface();
}

void
MainThread::work() 
{
    setMaxHeartbeatInterval( 1.0 );

    while ( !isStopping() )
    {
        try 
        {
            // this blocks until new data arrives
            readData();
            
            rangerArrayInterface_->localSetAndSend( orcaRangerArrayData_ );
            if ( hydroRangerArrayData_.haveWarnings )
            {
                health().warning( hydroRangerArrayData_.warnings );
            }
            else
            {
                health().ok();
            }

            stringstream ss;
            ss << "MainThread: Read ranger array data: " << ifaceutil::toString(orcaRangerArrayData_);
            context_.tracer().debug( ss.str(), 5 );
        }
        catch ( ... ) 
        {
            orcaice::catchMainLoopExceptions( health() );

            // Re-initialise the driver, unless we are stopping
            if ( !isStopping() ) {
                initHardwareDriver();
            }
        }

    } // end of while
}

void
MainThread::finalise() 
{
    // Hardware will be shut down in the driver's destructor.
}

////////////////////

void
MainThread::initHardwareDriver()
{
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    // Dynamically load the library and find the factory
    std::string driverLibName =
        orcaice::getPropertyWithDefault( prop, prefix+"DriverLib", "libHydroRangerArrayPlayerSonarClient.so" );
    context_.tracer().debug( "MainThread: Loading driver library "+driverLibName, 4 );
    // The factory which creates the driver
    std::auto_ptr<hydrointerfaces::RangerArrayFactory> driverFactory;

    try {
        driverLib_.reset( new hydrodll::DynamicallyLoadedLibrary(driverLibName) );
        driverFactory.reset( 
            hydrodll::dynamicallyLoadClass<hydrointerfaces::RangerArrayFactory,DriverFactoryMakerFunc>
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
            driver_.reset( driverFactory->createDriver( context_.toHydroContext() ) );
            break;
        }
        catch ( ... ) {
            orcaice::catchExceptionsWithStatusAndSleep( "initialising hardware driver", health() );
        }   
    }
    
    //
    // SENSOR DESCRIPTION
    //
    description_.timeStamp = orcaice::getNow();

    // obtain it from the driver
    hydrointerfaces::RangerArray::Description driverDescr;
    driver_->getDescription( driverDescr );

    // transfer internal sensor description
    int numberOfSamples = (int) driverDescr.poses.size();
    description_.minRange        = driverDescr.minRange;
    description_.maxRange        = driverDescr.maxRange;
    description_.detectionConeAngle  = driverDescr.detectionConeAngle;
    for( int i = 0; i < numberOfSamples; i ++ )
    {
	orca::Frame3d pose;
	orcabros1::convert( driverDescr.poses[i], pose );
	description_.poses.push_back( pose );
    }

    //
    // PREPARE SOME DATA FOR PUBLISHING
    //
    // Set up the scan objects
    orcaRangerArrayData_ = new orca::RangerArrayData;
    orcaRangerArrayData_->ranges.resize( numberOfSamples );
    orcaRangerArrayData_->timeStamps.resize( numberOfSamples );
    auxDataTimeStampsSec_.resize( numberOfSamples );
    auxDataTimeStampsUsec_.resize( numberOfSamples );

    // Point the pointers in hydroRangerArrayData_ at orcaRangerArrayData_
    hydroRangerArrayData_.ranges      = &(orcaRangerArrayData_->ranges[0]);
    hydroRangerArrayData_.timeStampsSec = &(auxDataTimeStampsSec_[0]);
    hydroRangerArrayData_.timeStampsUsec = &(auxDataTimeStampsUsec_[0]);
}

void
MainThread::initNetworkInterface()
{
    // activate component's communication
    activate( context_, this, subsysName() );

    //
    // EXTERNAL PROVIDED INTERFACE
    //
    rangerArrayInterface_ = new orcaifaceimpl::RangerArrayImpl( description_, "RangerArray", context_ );
    // init
    rangerArrayInterface_->initInterface( this, subsysName() );
}

void
MainThread::readData()
{
    //
    // Read from the driver
    //
    hydroRangerArrayData_.haveWarnings = false;
    driver_->read( hydroRangerArrayData_ );

    // transform timestamp data
    for ( int i = 0; i < (int) description_.poses.size(); i ++ )
    {
        orcaRangerArrayData_->timeStamps[i].seconds = auxDataTimeStampsSec_[i];
        orcaRangerArrayData_->timeStamps[i].useconds = auxDataTimeStampsUsec_[i];
    }
}
