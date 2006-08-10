#include <orcaice/orcaice.h>

#include "component.h"
#include "rangescannerconsumerI.h"
#include "handler.h"
#include "laser2og.h"
#include "ogsensormodel.h"

using namespace std;
using namespace orca;
using namespace laser2og;

Component::Component()
    : orcaice::Component( "Laser2Og", orcaice::HomeInterface  ),
    OggerDumbPtr_(NULL)
{
}

Component::~Component()
{
    // do not delete inputLoop_!!! It derives from Ice::Thread and deletes itself.
    if(OggerDumbPtr_!=NULL)
        delete OggerDumbPtr_;
}

// NOTE: this function returns after it's done, all variable that need to be permanet must
//       be declared as member variables.
void
Component::start()
{

    // get config
    Ice::PropertiesPtr prop = properties();
    std::string prefix = tag();
    prefix += ".Config.";

    // configure the sensor model
    OgLaserModelConfig sensorConfig;

    //sensorConfig.rangeMax = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Sensor.RangeMax", 8.1 );
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

    // Connect directly to the range scanner interface
    // TODO: this will not actually quit on ctrl-c
    while ( true ) //isActive() )
    {
        try
        {
            orcaice::connectToInterfaceWithTag<orca::RangeScannerPrx>( context(), rangeScannerPrx_, "RangeScanner" );
            break;
        }
        catch ( const orcaice::NetworkException & e )
        {
            tracer()->error( "failed to connect to remote object. Will try again after 3 seconds." );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
        // NOTE: connectToInterfaceWithTag() can also throw ConfigFileException,
        //       but if this happens it's ok if we just quit.
    }

    // Connect directly to the localise2d interface
    // TODO: this will not actually quit on ctrl-c
    while ( true ) //isActive() )
    {
        try
        {
            orcaice::connectToInterfaceWithTag<orca::Localise2dPrx>( context(), localise2dPrx_, "Localisation" );
            break;
        }
        catch ( const orcaice::NetworkException & e )
        {
            tracer()->error( "failed to connect to remote object. Will try again after 3 seconds." );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
    }

    // Connect directly to the ogFusion interface
    // TODO: this will not actually quit on ctrl-c
    while ( true ) //isActive() )
    {
        try
        {
            orcaice::connectToInterfaceWithTag<orca::OgFusionPrx>( context(), ogFusionPrx_, "OgFusion" );
            break;
        }
        catch ( const orcaice::NetworkException & e )
        {
            tracer()->error( "failed to connect to remote object. Will try again after 3 seconds." );
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
        orcaice::createConsumerInterface<orca::RangeScannerConsumerPrx>( context(), consumer );

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
        context().tracer()->error( errString );
        throw orcaice::Exception( ERROR_INFO, errString );
        return;
    }

    //
    // ENABLE NETWORK CONNECTIONS
    //
    // this may throw an exception which will be caught in Application
    // but will cause the app to exit
    activate();
    
    //
    // Subscribe for data
    //
    // will try forever until the user quits with ctrl-c
    // TODO: this will not actually quit on ctrl-c
    while ( true ) //isActive() )
    {
        try
        {
            rangeScannerPrx_->subscribe( rangeScannerConsumerPrx_ );
            break;
        }
        catch ( const orca::SubscriptionFailedException & e )
        {
            tracer()->error( "failed to subscribe for data updates. Will try again after 3 seconds." );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(3));
        }
    }

    //
    // MAIN DRIVER LOOP
    //
    context().tracer()->debug( "entering handler_...",5 );

    OggerDumbPtr_=new Laser2Og(mapConfig,sensorConfig);

    handler_ = new Handler(rangeScannerDataBuffer_,
                            localise2dPrx_,
                            ogFusionPrx_,
                            *OggerDumbPtr_,
                            context() );

    handler_->start();
    
    // the rest is handled by the application/service
}

void
Component::stop()
{
      // Tell the main loop to stop
      if(handler_!=NULL){
	  handler_->stop();

	  IceUtil::ThreadControl tc = handler_->getThreadControl();

	  // Then wait for it
	  tc.join();

	  // When the ThreadControl object goes out of scope thread is also deleted
	  // how dumb ! in this case Thread is handler_...
      }
}
