#include <iostream>

#include <orca/rangescanner.h>
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

    std::string prefix = context_.tag() + ".Config.";

    // configure the sensor model
    OgLaserModelConfig sensorConfig;

    sensorConfig.size = orcaice::getPropertyAsIntWithDefault( context_.properties(), prefix+"Sensor.Size", 101 );
    sensorConfig.occupMax = orcaice::getPropertyAsDoubleWithDefault( context_.properties(), prefix+"Sensor.OccupMax", 0.575 );
    sensorConfig.emptyMax = orcaice::getPropertyAsDoubleWithDefault( context_.properties(), prefix+"Sensor.EmptyMax", 0.425);
    sensorConfig.rangeStDev = orcaice::getPropertyAsDoubleWithDefault( context_.properties(), prefix+"Sensor.RangeStDev", 0.1);
    sensorConfig.rangeStDevMax = orcaice::getPropertyAsDoubleWithDefault( context_.properties(), prefix+"Sensor.RangeStDevMax", 3.0);
    sensorConfig.posStDevMax = orcaice::getPropertyAsDoubleWithDefault( context_.properties(), prefix+"Sensor.PosStDevMax", 5.0);
    sensorConfig.hedStDevMax = DEG2RAD( orcaice::getPropertyAsDoubleWithDefault( context_.properties(), prefix+"Sensor.HedStDevMax", 3.0) );

    //
    // REQUIRED INTERFACES: Laser, Localise2d, OgFusion
    //

    while ( isActive() )
    {
        try
        {
            orcaice::connectToInterfaceWithTag<orca::RangeScannerPrx>( context_, rangeScannerPrx_, "RangeScanner" );
            break;
        }
        catch ( const orcaice::NetworkException & e )
        {
            context_.tracer()->error( "failed to connect to remote object. Will try again after 3 seconds." );
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
            context_.tracer()->error( "failed to connect to remote object. Will try again after 3 seconds." );
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
            context_.tracer()->error( "failed to connect to remote object. Will try again after 3 seconds." );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
    }
    
    // Get the geometry
    RangeScannerGeometryPtr rangeScannerGeometry = rangeScannerPrx_->getGeometry();
    cout << "Range Scanner Geometry: " << orcaice::toString(rangeScannerGeometry) << endl;

    // Get the configuration
    RangeScannerConfigPtr rangeScannerConfig = rangeScannerPrx_->getConfig();
    cout << "Laser Config:   " << orcaice::toString(rangeScannerConfig) << endl;

    sensorConfig.rangeMax = rangeScannerConfig->maxRange;
    sensorConfig.angleIncrement = rangeScannerConfig->angleIncrement;

    // No need to getData()

    // create a callback object to recieve scans
    Ice::ObjectPtr consumer = new RangeScannerConsumerI(rangeScannerDataBuffer_);
    rangeScannerConsumerPrx_ =
            orcaice::createConsumerInterface<orca::RangeScannerConsumerPrx>( context_, consumer );

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
    try
    {
	RangeScannerDataPtr rangeScan = new RangeScannerData;
	Localise2dDataPtr pose = new Localise2dData;

	OgFusionDataPtr obs = new OgFusionData;

    //
    // IMPORTANT: Have to keep this loop rolling, because the 'isActive()' call checks for requests to shut down.
    //            So we have to avoid getting stuck in a loop anywhere within this main loop.
    //
    while ( isActive() )
	{
	    int ret=rangeScannerDataBuffer_.getAndPopNext(rangeScan,1000);
	    if(ret==0)
        {
		    try
            {
                pose=localise2dPrx_->getDataAtTime(rangeScan->timeStamp);
            }
            catch( orca::DataNotExistException e )
            {
                cout << "ERROR(handler.cpp): could not fetch pose\n";
                cout << "ERROR(handler.cpp): reason: " << e.what << endl;
            }

            laser2Og_->process(*pose,*rangeScan);
            laser2Og_->getObs(obs->observation);
            obs->timeStamp = rangeScan->timeStamp;
            
            //send out OgFusionData
            ogFusionPrx_->setData(obs);

	    }
	    else if (ret==1)
        {
            cout << "TRACE(handler.cpp): Interrupted\n";
	    }
        else
        {
                //timeout
	    }
	}
    
    }   // end of try
    catch ( Ice::CommunicatorDestroyedException &e )
    {
        // This is OK: it means that the communicator shut down (eg via Ctrl-C)
        // somewhere in mainLoop.
        //
        // Could probably handle it better for an Application by stopping the component on Ctrl-C
        // before shutting down communicator.
    }
    context_.tracer()->debug( "dropping out from run()", 5 );
}

