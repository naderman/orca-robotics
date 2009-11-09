/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp,
 *               George Mathews, Stef Williams.
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaobj/rangescanner2d.h>
#include <hydrofeatureobs/hydrofeatureobs.h>
#include <orcafeatureobs/orcafeatureobs.h>

#include "mainthread.h"

using namespace std;
using namespace laserfeatures;

const double MAX_HEARTBEAT_INITIALIZIING_DRIVER = 10.0;
const double MAX_HEARTBEAT_WORKING_DRIVER = 1.0;

MainThread::MainThread( const orcaice::Context &context ) :
    SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    laserConsumer_(new orcaifaceimpl::StoringRangeScanner2dConsumerImpl(context)),
    context_(context)
{
}

void 
MainThread::initialise()
{
    setMaxHeartbeatInterval( MAX_HEARTBEAT_INITIALIZIING_DRIVER );

    laserConsumer_->subscribeWithTag( "Laser", this, subsysName() );
    orcaice::getDescriptionWithTag<orca::LaserScanner2dPrx,orca::RangeScanner2dDescription>
        ( context_, "Laser", laserDescr_, this, subsysName() );
    initDriver();
    initPolarFeatureInterface();
}

void 
MainThread::work()
{
    setMaxHeartbeatInterval( MAX_HEARTBEAT_WORKING_DRIVER );

    // Temporaries for use in loop
    orca::PolarFeature2dData    featureData;
    orca::LaserScanner2dDataPtr laserData;
    orca::RangeScanner2dDataPtr rangeData;
    std::vector<hydrofeatureobs::FeatureObs*> hydroFeatures;

    // Loop forever till we get shut down.
    while ( !isStopping() )
    {
        // this try makes this component robust to exceptions
        try
        {                
            //
            // block on arrival of laser data
            //
            const int timeoutMs = 1000;
            int ret = laserConsumer_->store().getNext( rangeData, timeoutMs );
            if ( ret != 0 ) {
                if ( isStopping() || context_.isDeactivating() ) {
                    throw orcaice::ComponentDeactivatingException( ERROR_INFO, "Failed to get inputs because the component is deactivating" );
                }
                // alexm: not calling it a fault
                stringstream ss;
                ss << "Timed out (" << timeoutMs << "ms) waiting for laser data.  Reconnecting.";
//                 health().fault( ss.str() );
                health().warning( ss.str() );
                // TODO: on a busy system data is sometimes delayed, usually there's no need to resubscribe.
                // we should profile delay distribution and possibly wait longer for resubscription.
                laserConsumer_->subscribeWithTag( "Laser", this, subsysName() );
                continue;
            }

            // Assume that what we're really getting is laser data
            laserData = orca::LaserScanner2dDataPtr::dynamicCast( rangeData );
            
            if ( (int)(laserData->ranges.size()) != laserDescr_.numberOfSamples )
            {
                stringstream ss;
                ss << "Got laser scan: expected " << laserDescr_.numberOfSamples
                    << " returns, got " << laserData->ranges.size();
                health().warning( ss.str() );
                continue;
            }

            //
            // execute algorithm to compute features
            //
            try {
                hydroFeatures = driver_->extractFeatures( laserData->ranges, laserData->intensities );
            }
            catch ( std::exception &e )
            {
                stringstream ss;
                ss << "Exception while extracting features: " << e.what();
                throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
            }

            // Convert format
            featureData.features.clear();
            for ( size_t i=0; i < hydroFeatures.size(); i++ )
            {
                featureData.features.push_back( orcafeatureobs::convert( *hydroFeatures[i] ) );
                delete hydroFeatures[i];
            }

            // features have the same time stamp as the raw scan
            featureData.timeStamp = laserData->timeStamp;

            // Tell the world
            featureInterface_->localSetAndSend( featureData );

            health().ok();
        } // try
        catch ( ... ) 
        {
            orcaice::catchMainLoopExceptions( health() );
        }
    } // while
}

// NOTE: this function is documented more than usual.
// Use it as a reference for understanding/writing components which
// load Hydro drivers.
void 
MainThread::initDriver()
{
    setMaxHeartbeatInterval( MAX_HEARTBEAT_INITIALIZIING_DRIVER );

    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    //
    // Dynamically load the library and find the factory
    //

    // This just gets the name (or possibly the path) of the shared library
    // (*.so) which will be loaded.
    std::string driverLibName = 
        orcaice::getPropertyWithDefault( prop, prefix+"DriverLib", "libHydroLaserFeatureExtractorCombined.so" );
    context_.tracer().debug( "MainThread: Loading driver library "+driverLibName, 4 );

    // The factory which creates the driver
    // The auto-pointer to the "factory" which is just a class which has a
    // a function createDriver() -- we'll use it a bit later.
    // See http://en.wikipedia.org/wiki/Auto_ptr
    std::auto_ptr<hydrointerfaces::LaserFeatureExtractorFactory> driverFactory;
    try {
        //
        // this reset() function is a standard function of the auto-pointer.
        // If we were using a normal "dumb" pointer like this:
        //    hydrointerfaces::LaserFeatureExtractorFactory* driverFactory;
        // the reset() function is equivalent to assignment of the dumb pointer:
        //    driverFactory=new hydrodll::DynamicallyLoadedLibrary(driverLibName);
        //
        driverLib_.reset( new hydrodll::DynamicallyLoadedLibrary(driverLibName) );

        //
        // now comes some magic...
        // The templated function hydrodll::dynamicallyLoadClass() goes inside
        // the code contained in the shared library driverLib_ and creates an
        // instance of class returned by the C function createLaserFeatureExtractorDriverFactory()
        // This function is defined at the bottom of                
        //   [HYDRO]/src/hydrodrivers/hydrolaserfeatureextractor/driver.h
        //
        driverFactory.reset( 
            hydrodll::dynamicallyLoadClass<hydrointerfaces::LaserFeatureExtractorFactory,LaserFeatureExtractorDriverFactoryMakerFunc>
            ( *driverLib_, "createLaserFeatureExtractorDriverFactory" ) );
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
            context_.tracer().info( "MainThread: Creating driver..." );

            //
            // Because we are in a while loop, we can come here already with 
            // a created driver. We reset the auto pointer to 0, so that the 
            // previously created driver is destructed.
            // (I think this resetting to zero is unnecessary, the following line should have
            // the same effect.)
            //
            driver_.reset(0);
            // Create the driver calling the special function of the "factory".
            driver_.reset( driverFactory->createDriver( laserDescr_.maxRange,
                                                        laserDescr_.startAngle,
                                                        orcaobj::calcAngleIncrement( laserDescr_.fieldOfView, 
                                                                                     laserDescr_.numberOfSamples ),
                                                        laserDescr_.numberOfSamples,
                                                        context_.toHydroContext() ) );
            break;
        }
        catch ( ... ) {
            //
            // use the special orca wrapping function which catches all exceptions, 
            // sleeps for 1 sec (default), and goes on to try again.
            //
            orcaice::catchExceptionsWithStatusAndSleep( "initialising algorithm driver", health() );
        }
    }

    setMaxHeartbeatInterval( MAX_HEARTBEAT_WORKING_DRIVER );
}

void
MainThread::initPolarFeatureInterface()
{    
    //
    // Instantiate External Interface
    //
    orca::PolarFeature2dDescription descr;
    descr.timeStamp = orcaice::getNow();
    descr.offset = laserDescr_.offset;
    featureInterface_ = new orcaifaceimpl::PolarFeature2dImpl( descr, "PolarFeature2d", context_ );

    // init
    featureInterface_->initInterface( this, subsysName() );
}
