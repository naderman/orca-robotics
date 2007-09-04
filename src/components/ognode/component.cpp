/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include <orcaice/orcaice.h>
#include <orcaogfusion/orcaogfusion.h>

#include "component.h"
#include "handler.h"
#include "ogfusionI.h"

using namespace std;
using namespace orca;
using namespace ognode;

Component::Component()
    : orcaice::Component( "OgNode", orcaice::HomeInterface  ),
    OgFusionDataBuffer_(10,orcaice::BufferTypeCircular)
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

    mapConfig.mapResX = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Map.ResX", 0.5 );
    mapConfig.mapResY = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Map.ResY", 0.5 );
    
    double sizeXMetres = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Map.SizeXMetres", 50.0 );
    double sizeYMetres = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Map.SizeYMetres", 50.0 );
    
    mapConfig.mapSizeX = (int)floor(sizeXMetres/mapConfig.mapResX);
    mapConfig.mapSizeY = (int)floor(sizeYMetres/mapConfig.mapResY);

    mapConfig.mapOriginX=orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Map.OriginX", -25.0 );
    mapConfig.mapOriginY=orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Map.OriginY", -25.0 );

    mapConfig.mapOrientation=orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Map.Orientation", 0.0 );

    // setup local map
    localMap_.reallocate(mapConfig.mapSizeX, mapConfig.mapSizeY);
    localMap_.offset().p.x=mapConfig.mapOriginX;
    localMap_.offset().p.y=mapConfig.mapOriginY;
    localMap_.offset().o=mapConfig.mapOrientation;
    localMap_.setMetresPerCellX(mapConfig.mapResX);
    localMap_.setMetresPerCellY(mapConfig.mapResY);
    localMap_.fill(orcaogmap::CELL_UNKNOWN);

    // duplicate map config for OgFusion Interface
    OgFusionConfig ogFusionConfig;
    ogFusionConfig.offset.p.x=mapConfig.mapOriginX;
    ogFusionConfig.offset.p.y=mapConfig.mapOriginY;
    ogFusionConfig.offset.o=mapConfig.mapOrientation;
    ogFusionConfig.numCellsX=mapConfig.mapSizeX;
    ogFusionConfig.numCellsY=mapConfig.mapSizeY;
    ogFusionConfig.metresPerCellX=mapConfig.mapResX;
    ogFusionConfig.metresPerCellY=mapConfig.mapResY;

    // create an initial map to pass to OgMapI
    OgMapData map;
    convert( localMap_, map );

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


