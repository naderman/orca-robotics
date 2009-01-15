/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp,
 *               George Mathews, Stef Williams.
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h>
#include <hydrofeatureobs/hydrofeatureobs.h>

#include "mainthread.h"

using namespace std;
using namespace laserfeatures;

namespace {

    // conversion between Orca and Hydro datatypes
    orca::SinglePolarFeature2dPtr convert( const hydrofeatureobs::FeatureObs &hydroFeature )
    {
        orca::SinglePolarFeature2dPtr orcaFeature;

        {
            const hydrofeatureobs::PointFeatureObs *f =
                dynamic_cast<const hydrofeatureobs::PointFeatureObs*>(&hydroFeature);
            if ( f != NULL )
            {
                orca::PointPolarFeature2d *orcaF = new orca::PointPolarFeature2d;
                orcaFeature = orcaF;
                orcaF->p.r       = f->range();
                orcaF->p.o       = f->bearing();
                orcaF->rangeSd   = f->rangeSd();
                orcaF->bearingSd = f->bearingSd();
            }
        }
        {
            const hydrofeatureobs::PoseFeatureObs *f =
                dynamic_cast<const hydrofeatureobs::PoseFeatureObs*>(&hydroFeature);
            if ( f != NULL )
            {
                orca::PosePolarFeature2d *orcaF = new orca::PosePolarFeature2d;
                orcaFeature = orcaF;
                orcaF->p.r           = f->range();
                orcaF->p.o           = f->bearing();
                orcaF->orientation   = f->orientation();
                orcaF->rangeSd       = f->rangeSd();
                orcaF->bearingSd     = f->bearingSd();
                orcaF->orientationSd = f->orientationSd();
            }
        }
        {
            const hydrofeatureobs::LineFeatureObs *f =
                dynamic_cast<const hydrofeatureobs::LineFeatureObs*>(&hydroFeature);
            if ( f != NULL )
            {
                orca::LinePolarFeature2d *orcaF = new orca::LinePolarFeature2d;
                orcaFeature = orcaF;
                orcaF->start.r      = f->rangeStart();
                orcaF->start.o      = f->bearingStart();
                orcaF->end.r        = f->rangeEnd();
                orcaF->end.o        = f->bearingEnd();
                orcaF->startSighted = f->startSighted();
                orcaF->endSighted   = f->endSighted();
                orcaF->rhoSd        = f->rhoSd();
                orcaF->alphaSd      = f->alphaSd();
            }
        }
        
        orcaFeature->type           = hydroFeature.featureType();
        orcaFeature->pFalsePositive = hydroFeature.pFalsePositive();
        orcaFeature->pTruePositive  = hydroFeature.pTruePositive();

        return orcaFeature;
    }

}

////////////////////////////////////////////////////////////////////////////////

MainThread::MainThread( const orcaice::Context &context ) :
    SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    laserConsumer_(new orcaifaceimpl::StoringRangeScanner2dConsumerImpl(context)),
    context_(context)
{
}

void 
MainThread::initialise()
{
    subStatus().setMaxHeartbeatInterval( 10.0 );

    // These functions catch their exceptions.
    activate( context_, this, subsysName() );

    connectToLaser();
    getLaserDescription();
    initDriver();
    initPolarFeatureInterface();
}

void 
MainThread::work()
{
    subStatus().setMaxHeartbeatInterval( 3.0 );

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
//                 subStatus().fault( ss.str() );
                subStatus().warning( ss.str() );
                connectToLaser();
                continue;
            }

            // Assume that what we're really getting is laser data
            laserData = orca::LaserScanner2dDataPtr::dynamicCast( rangeData );
            
            if ( (int)(laserData->ranges.size()) != laserDescr_.numberOfSamples )
            {
                stringstream ss;
                ss << "Got laser scan: expected " << laserDescr_.numberOfSamples
                    << " returns, got " << laserData->ranges.size();
                subStatus().warning( ss.str() );
                continue;
            }

            //
            // execute algorithm to compute features
            //
            hydroFeatures = driver_->extractFeatures( laserData->ranges, laserData->intensities );

            // Convert format
            featureData.features.clear();
            for ( size_t i=0; i < hydroFeatures.size(); i++ )
            {
                featureData.features.push_back( convert( *hydroFeatures[i] ) );
                delete hydroFeatures[i];
            }

            // features have the same time stamp as the raw scan
            featureData.timeStamp = laserData->timeStamp;

            // Tell the world
            featureInterface_->localSetAndSend( featureData );

            subStatus().ok();
        } // try
        catch ( ... ) 
        {
            orcaice::catchMainLoopExceptions( subStatus() );
        }
    } // while
}

void 
MainThread::initDriver()
{
    subStatus().setMaxHeartbeatInterval( 10.0 );

    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    // Dynamically load the library and find the factory
    std::string driverLibName = 
        orcaice::getPropertyWithDefault( prop, prefix+"DriverLib", "libHydroLaserFeatureExtractorCombined.so" );
    context_.tracer().debug( "MainThread: Loading driver library "+driverLibName, 4 );
    // The factory which creates the driver
    std::auto_ptr<hydrointerfaces::LaserFeatureExtractorFactory> driverFactory;
    try {
        driverLib_.reset( new hydrodll::DynamicallyLoadedLibrary(driverLibName) );
        driverFactory.reset( 
            hydrodll::dynamicallyLoadClass<hydrointerfaces::LaserFeatureExtractorFactory,DriverFactoryMakerFunc>
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
            context_.tracer().info( "MainThread: Creating driver..." );
            driver_.reset(0);
            driver_.reset( driverFactory->createDriver( laserDescr_.maxRange,
                                                        laserDescr_.startAngle,
                                                        orcaobj::calcAngleIncrement( laserDescr_.fieldOfView, 
                                                                                     laserDescr_.numberOfSamples ),
                                                        context_.toHydroContext() ) );
            break;
        }
        catch ( ... ) {
            orcaice::catchExceptionsWithStatusAndSleep( "initialising algorithm driver", subStatus() );
        }
    }

    subStatus().setMaxHeartbeatInterval( 1.0 );
}

void 
MainThread::connectToLaser()
{
    while ( !isStopping() )
    {
        try {
            laserConsumer_->subscribeWithTag( "Laser", this, subsysName() );
            break;
        }
        catch ( ... ) {
            orcaice::catchExceptionsWithStatusAndSleep( "connecting to laser", subStatus() );
        }       
    }
}

void 
MainThread::getLaserDescription()
{
    // Laser proxy
    orca::LaserScanner2dPrx laserPrx;
    
    while ( !isStopping() )
    {
        try
        {
            context_.tracer().debug( "Getting laser description...", 2 );
            orcaice::connectToInterfaceWithTag( context_, laserPrx, "Laser" );
            laserDescr_ = laserPrx->getDescription();
            stringstream ss;
            ss << "Got laser description: " << orcaobj::toString( laserDescr_ );
            context_.tracer().info( ss.str() );
            break;
        }
        catch ( ... ) {
            orcaice::catchExceptionsWithStatusAndSleep( "getting laser description", subStatus() );
        }
    }
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
