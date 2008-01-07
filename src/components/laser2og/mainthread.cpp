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

#include <orca/rangescanner2d.h>
#include <orca/localise2d.h>
#include <orca/ogfusion.h>
#include <orcaice/orcaice.h>

#include "laser2og.h"
#include "mainthread.h"
#include "ogsensormodel.h"
#include "rangescannerconsumerI.h"
// this file was in libOrcaMisc before it was disbanded.
// there's another version in libHydroNavUtil, we should probably use that one
#include "cov2d.h"

using namespace std;
using namespace orca;
using namespace laser2og;

namespace {

    // Returns isPoseClear:
    //   true:  worked out pose OK
    //   false: localisation is too uncertain.
    bool calcPose( const orca::Localise2dData &localiseData,
                   hydronavutil::Pose &pose,
                   double maxPositionSd,
                   double maxHeadingSd )
    {
        if ( localiseData.hypotheses.size() > 1 )
        {
            cout<<"TRACE(handler.cpp): Can only deal with exactly one localisation hypothesis.  Found: " 
                << localiseData.hypotheses.size() << endl;
            return false;
        }

        // Ellipse of position uncertainty
        laser2og::Cov2d posEll(localiseData.hypotheses[0].cov.xx,
                               localiseData.hypotheses[0].cov.xy,
                               localiseData.hypotheses[0].cov.yy );

        double a,b,t;
        posEll.ellipse(a,b,t);

        // Find larger of two components
        double posStDev = a > b ? sqrt(a) : sqrt(b);

        if(posStDev > maxPositionSd )
        {
            cout << "WARNING(handler.cpp)::calcPose(): position std dev " << posStDev << " m is too big." << endl;
            return false;
        }

        // check heading uncertainty
        double headingStdDev = sqrt(localiseData.hypotheses[0].cov.tt);

        if( headingStdDev > maxHeadingSd )
        {
            cout << "WARNING(handler.cpp)::calcPose(): heading std dev " << RAD2DEG(headingStdDev) << " deg is too big." <<endl;
            return false;
        }

        // Localisation is certain enough.
        pose = hydronavutil::Pose( localiseData.hypotheses[0].mean.p.x,
                                  localiseData.hypotheses[0].mean.p.y,
                                  localiseData.hypotheses[0].mean.o );
        return true;
    }

}

//////////////////////////////////////////////////////////////////////

MainThread::MainThread( const orcaice::Context &context ) :
    SafeThread(context.tracer()),
    context_(context),
    rangeScannerDataBuffer_(-1,hydroiceutil::BufferTypeCircular),
    laser2Og_(0)
{
}

MainThread::~MainThread()
{
    if (laser2Og_!=0)
        delete laser2Og_;
}

void
MainThread::init()
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

    while ( !isStopping() )
    {
        try
        {
            orcaice::connectToInterfaceWithTag<orca::RangeScanner2dPrx>( context_, rangeScannerPrx_, "Observations" );
            break;
        }
        catch ( const orcaice::NetworkException & e )
        {
            std::stringstream ss;
            ss << "Failed to connect to remote rangeScanner object: " << e.what()
               << "Will try again after 3 seconds.";
            context_.tracer().error( ss.str() );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
            // NOTE: connectToInterfaceWithTag() can also throw ConfigFileException,
            //       but if this happens it's ok if we just quit.
    }

    while ( !isStopping() )
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
            context_.tracer().error( ss.str() );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
    }

    while ( !isStopping() )
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
            context_.tracer().error( ss.str() );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
    }
    
    // Get the configuration
    RangeScanner2dDescription descr = rangeScannerPrx_->getDescription();
    cout << orcaice::toString(descr) << endl;

    sensorConfig.rangeMax = descr.maxRange;
    sensorConfig.angleIncrement = descr.fieldOfView/(descr.numberOfSamples+1);

    // create a callback object to recieve scans
    Ice::ObjectPtr consumer = new RangeScanner2dConsumerI(rangeScannerDataBuffer_);
    rangeScannerConsumerPrx_ =
            orcaice::createConsumerInterface<orca::RangeScanner2dConsumerPrx>( context_, consumer );

    OgFusionConfig ogFusionConfig = ogFusionPrx_->getConfig();

    ogfusion::MapConfig mapConfig;

    // read map info from config
    mapConfig.mapSizeX = ogFusionConfig.numCellsX;
    mapConfig.mapSizeY = ogFusionConfig.numCellsY;
    mapConfig.mapResX = ogFusionConfig.metresPerCellX;
    mapConfig.mapResY = ogFusionConfig.metresPerCellY;
    mapConfig.mapOriginX = ogFusionConfig.offset.p.x;
    mapConfig.mapOriginY = ogFusionConfig.offset.p.y;
    mapConfig.mapOrientation = ogFusionConfig.offset.o;

    if(ogFusionConfig.offset.o != 0.0){
        std::string errString = "Laser2Og currently only support axis aligned OgMaps";
        context_.tracer().error( errString );
        throw hydroutil::Exception( ERROR_INFO, errString );
        return;
    }

    
    //
    // Subscribe for data
    //
    // will try forever until the user quits with ctrl-c
    while ( !isStopping() )
    {
        try
        {
            rangeScannerPrx_->subscribe( rangeScannerConsumerPrx_ );
            break;
        }
        catch ( const orca::SubscriptionFailedException & e )
        {
            context_.tracer().error( "failed to subscribe for data updates. Will try again after 3 seconds." );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
    }
    
    laser2Og_ = new Laser2Og(mapConfig,sensorConfig);

}

void
MainThread::walk()
{
    //
    // ENABLE NETWORK CONNECTIONS
    //
    // multi-try function
    orcaice::activate( context_, this );

    init();

	RangeScanner2dDataPtr rangeScan = new RangeScanner2dData;
	Localise2dData localiseData;

    //
    // IMPORTANT: Have to keep this loop rolling, because the '!isStopping()' call checks for requests to shut down.
    //            So we have to avoid getting stuck in a loop anywhere within this main loop.
    //
    while ( !isStopping() )
	{
        try
        {
            
            while ( !isStopping() )
            {
                int ret=rangeScannerDataBuffer_.getAndPopNext(rangeScan,1000);
                if(ret!=0) {
                    context_.tracer().info("no range scan available: waiting ...");
                } else {
                    break;
                }
            }
                
            try
            {
                localiseData = localise2dPrx_->getData();
            }
            catch( orca::DataNotExistException e )
            {
                std::stringstream ss;
                ss << "handler.cpp: run: could not fetch pose because of: " << e.what;
                context_.tracer().warning( ss.str() );
                throw;
            }
    
            // TODO: could be more accurate by interpolating here...
            // TODO: add laser offset
            hydronavutil::Pose pose;
            bool isPoseClear = calcPose( localiseData,
                                         pose,
                                         laser2Og_->positionStdDevMax(),
                                         laser2Og_->headingStdDevMax() );
            
            if ( isPoseClear )
            {
                OgFusionData obs;
                obs.observation = laser2Og_->process(pose,*rangeScan);
                obs.timeStamp   = rangeScan->timeStamp;
            
                //send out OgFusionData
                ogFusionPrx_->setData(obs);
            }
            
        }   // end of try
        catch ( orca::DataNotExistException e )
        {
            stringstream ss;
            ss << "handler.cpp: run: DataNotExistException, reason: " << e.what;
            context_.tracer().warning( ss.str() );
        }
        catch ( const orca::OrcaException & e )
        {
            stringstream ss;
            ss << "unexpected (remote?) orca exception: " << e << ": " << e.what;
            context_.tracer().error( ss.str() );
        }
        catch ( const hydroutil::Exception & e )
        {
            stringstream ss;
            ss << "unexpected (local?) orcaice exception: " << e.what();
            context_.tracer().error( ss.str() );
        }
        catch ( const Ice::Exception & e )
        {
            stringstream ss;
            ss << "unexpected Ice exception: " << e;
            context_.tracer().error( ss.str() );
        }
        catch ( const std::exception & e )
        {
        // once caught this beast in here, don't know who threw it 'St9bad_alloc'
            stringstream ss;
            ss << "unexpected std exception: " << e.what();
            context_.tracer().error( ss.str() );
        }
        catch ( ... )
        {
            context_.tracer().error( "unexpected exception from somewhere.");
        }
        
    } // end of main loop
}
