/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include <iostream>

#include <orca/rangescanner2d.h>
#include <orca/localise2d.h>
#include <orca/ogfusion.h>
#include <orcaice/orcaice.h>

#include "laser2og.h"
#include "handler.h"
#include "ogsensormodel.h"
#include "rangescannerconsumerI.h"

using namespace std;
using namespace orca;
using namespace laser2og;

Handler::Handler( const orcaice::Context &context )
    :   context_(context),
        laser2Og_(0)
{
    init();
}

Handler::~Handler()
{
    if (laser2Og_!=0)
        delete laser2Og_;
}

void
Handler::init()
{
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    // configure the sensor model
    OgLaserModelConfig sensorConfig;

    sensorConfig.size = orcaice::getPropertyAsIntWithDefault( prop, prefix+"Sensor.Size", 101 );
    sensorConfig.occupMax = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Sensor.OccupMax", 0.575 );
    sensorConfig.emptyMax = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Sensor.EmptyMax", 0.425);
    sensorConfig.rangeStDev = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Sensor.RangeStDev", 0.1);
    sensorConfig.rangeStDevMax = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Sensor.RangeStDevMax", 3.0);
    sensorConfig.posStDevMax = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Sensor.PosStDevMax", 5.0);
    sensorConfig.hedStDevMax = DEG2RAD( orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Sensor.HedStDevMax", 3.0) );

    //
    // REQUIRED INTERFACES: Laser, Localise2d, OgFusion
    //

    while ( isActive() )
    {
        try
        {
            orcaice::connectToInterfaceWithTag<orca::RangeScanner2dPrx>( context_, rangeScannerPrx_, "RangeScanner" );
            break;
        }
        catch ( const orcaice::NetworkException & e )
        {
            std::stringstream ss;
            ss << "Failed to connect to remote rangeScanner object: " << e.what()
               << "Will try again after 3 seconds.";
            context_.tracer()->error( ss.str() );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
            // NOTE: connectToInterfaceWithTag() can also throw ConfigFileException,
            //       but if this happens it's ok if we just quit.
    }

    while ( isActive() )
    {
        try
        {
            orcaice::connectToInterfaceWithTag<orca::Localise2dPrx>( context_, localise2dPrx_, "Localisation" );
            break;
        }
        catch ( const orcaice::NetworkException & e )
        {
            std::stringstream ss;
            ss << "Failed to connect to remote localise2d object: " << e.what()
               << "Will try again after 3 seconds.";
            context_.tracer()->error( ss.str() );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
    }

    while ( isActive() )
    {
        try
        {
            orcaice::connectToInterfaceWithTag<orca::OgFusionPrx>( context_, ogFusionPrx_, "OgFusion" );
            break;
        }
        catch ( const orcaice::NetworkException & e )
        {
            std::stringstream ss;
            ss << "Failed to connect to remote ogfusion object: " << e.what()
               << "Will try again after 3 seconds.";
            context_.tracer()->error( ss.str() );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
    }
    
    // Get the configuration
    RangeScanner2dDescriptionPtr descr = rangeScannerPrx_->getDescription();
    cout << orcaice::toString(descr) << endl;

    sensorConfig.rangeMax = descr->maxRange;
    sensorConfig.angleIncrement = descr->fieldOfView/(descr->numberOfReturns+1);

    // No need to getData()

    // create a callback object to recieve scans
    Ice::ObjectPtr consumer = new RangeScanner2dConsumerI(rangeScannerDataBuffer_);
    rangeScannerConsumerPrx_ =
            orcaice::createConsumerInterface<orca::RangeScanner2dConsumerPrx>( context_, consumer );

    OgFusionConfigPtr ogFusionConfig = ogFusionPrx_->getConfig();

    ogfusion::MapConfig mapConfig;

    // read map info from config
    mapConfig.mapSizeX = ogFusionConfig->numCellsX;
    mapConfig.mapSizeY = ogFusionConfig->numCellsY;
    mapConfig.mapResX = ogFusionConfig->metresPerCellX;
    mapConfig.mapResY = ogFusionConfig->metresPerCellY;
    mapConfig.mapOriginX = ogFusionConfig->origin.p.x;
    mapConfig.mapOriginY = ogFusionConfig->origin.p.y;
    mapConfig.mapOrientation = ogFusionConfig->origin.o;

    if(ogFusionConfig->origin.o != 0.0){
        std::string errString = "Laser2Og currently only support axis aligned OgMaps";
        context_.tracer()->error( errString );
        throw orcaice::Exception( ERROR_INFO, errString );
        return;
    }

    
    //
    // Subscribe for data
    //
    // will try forever until the user quits with ctrl-c
    while ( isActive() )
    {
        try
        {
            rangeScannerPrx_->subscribe( rangeScannerConsumerPrx_ );
            break;
        }
        catch ( const orca::SubscriptionFailedException & e )
        {
            context_.tracer()->error( "failed to subscribe for data updates. Will try again after 3 seconds." );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
    }
    
    laser2Og_ = new Laser2Og(mapConfig,sensorConfig);

}

void
Handler::run()
{

	RangeScanner2dDataPtr rangeScan = new RangeScanner2dData;
	Localise2dDataPtr pose = new Localise2dData;
	OgFusionDataPtr obs = new OgFusionData;

    //
    // IMPORTANT: Have to keep this loop rolling, because the 'isActive()' call checks for requests to shut down.
    //            So we have to avoid getting stuck in a loop anywhere within this main loop.
    //
    while ( isActive() )
	{
        try
        {
            
            while ( isActive() )
            {
                int ret=rangeScannerDataBuffer_.getAndPopNext(rangeScan,1000);
                if(ret!=0) {
                    context_.tracer()->info("no range scan available: waiting ...");
                } else {
                    break;
                }
            }
                
            try
            {
                pose=localise2dPrx_->getDataAtTime(rangeScan->timeStamp);
            }
            catch( orca::DataNotExistException e )
            {
                std::stringstream ss;
                ss << "handler.cpp: run: could not fetch pose because of: " << e.what;
                context_.tracer()->warning( ss.str() );
                throw;
            }
    
            laser2Og_->process(*pose,*rangeScan);
            laser2Og_->getObs(obs->observation);
            obs->timeStamp = rangeScan->timeStamp;
            
            //send out OgFusionData
            ogFusionPrx_->setData(obs);
            
        }   // end of try
        catch ( orca::DataNotExistException e )
        {
            stringstream ss;
            ss << "handler.cpp: run: DataNotExistException, reason: " << e.what;
            context_.tracer()->warning( ss.str() );
        }
        catch ( const orca::OrcaException & e )
        {
            stringstream ss;
            ss << "unexpected (remote?) orca exception: " << e << ": " << e.what;
            context_.tracer()->error( ss.str() );
        }
        catch ( const orcaice::Exception & e )
        {
            stringstream ss;
            ss << "unexpected (local?) orcaice exception: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        catch ( const Ice::Exception & e )
        {
            stringstream ss;
            ss << "unexpected Ice exception: " << e;
            context_.tracer()->error( ss.str() );
        }
        catch ( const std::exception & e )
        {
        // once caught this beast in here, don't know who threw it 'St9bad_alloc'
            stringstream ss;
            ss << "unexpected std exception: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        catch ( ... )
        {
            context_.tracer()->error( "unexpected exception from somewhere.");
        }
        
    } // end of main loop
    
    context_.tracer()->debug( "dropping out from run()", 5 );
    
    waitForStop();
}

