#include <orcaice/orcaice.h>
#include <orcaogfusion/orcaogfusion.h>

#include "component.h"
#include "handler.h"
#include "ogfusionI.h"

using namespace std;
using namespace orca;
using namespace ognode;

Component::Component()
    : orcaice::Component( "OgNode", orcaice::HomeInterface  )
{
}

Component::~Component()
{
}

void
Component::start()
{
    // get config
    Ice::PropertiesPtr prop = properties();
    std::string prefix = tag();
    prefix += ".Config.";

    ogfusion::MapConfig mapConfig;
    // read map info from config
    mapConfig.mapSizeX = orcaice::getPropertyAsIntWithDefault( prop, prefix+"Map.SizeX", 100 );
    mapConfig.mapSizeY = orcaice::getPropertyAsIntWithDefault( prop, prefix+"Map.SizeY", 100 );

    mapConfig.mapResX = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Map.ResX", 0.5 );
    mapConfig.mapResY = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Map.ResY", 0.5 );

    mapConfig.mapOriginX=orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Map.OriginX", -25.0 );
    mapConfig.mapOriginY=orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Map.OriginY", -25.0 );

    mapConfig.mapOrientation=orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Map.Orientation", 0.0 );

    // setup local map
    localMap_.reallocate(mapConfig.mapSizeX, mapConfig.mapSizeY);
    localMap_.origin().p.x=mapConfig.mapOriginX;
    localMap_.origin().p.y=mapConfig.mapOriginY;
    localMap_.origin().o=mapConfig.mapOrientation;
    localMap_.setMetresPerCellX(mapConfig.mapResX);
    localMap_.setMetresPerCellY(mapConfig.mapResY);
    localMap_.fill(orcaogmap::CELL_UNKNOWN);

    // duplicate map config for OgFusion Interface
    OgFusionConfigPtr ogFusionConfig = new OgFusionConfig;
    ogFusionConfig->origin.p.x=mapConfig.mapOriginX;
    ogFusionConfig->origin.p.y=mapConfig.mapOriginY;
    ogFusionConfig->origin.o=mapConfig.mapOrientation;
    ogFusionConfig->numCellsX=mapConfig.mapSizeX;
    ogFusionConfig->numCellsY=mapConfig.mapSizeY;
    ogFusionConfig->metresPerCellX=mapConfig.mapResX;
    ogFusionConfig->metresPerCellY=mapConfig.mapResY;

    // create an initial map to pass to OgMapI
    OgMapDataPtr map = new OgMapData;
    convert(localMap_,map);

    // create a callback object to recieve requests
    ogFusionObjPtr_ = new OgFusionI(ogFusionConfig, OgFusionDataBuffer_);
    orcaice::createInterfaceWithTag(context(), ogFusionObjPtr_, "OgFusion" );

    // create a callback object to recieve requests
    OgMapI *ogMapObjDumbPtr = new OgMapI(map , "OgMap" ,context());
    ogMapObjPtr_ = ogMapObjDumbPtr;
    orcaice::createInterfaceWithTag( context(), ogMapObjPtr_, "OgMap" );

    //
    // ENABLE NETWORK CONNECTIONS
    //
    // this may throw an exception which will be caught in Application
    // but will cause the app to exit
    activate();
    
    //
    // MAIN DRIVER LOOP
    //
    context().tracer()->debug( "entering handler_...",5 );

    handler_ = new Handler(*ogMapObjDumbPtr, OgFusionDataBuffer_, localMap_, context() );

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


