/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include <orcaogmap/orcaogmap.h>
#include <orca/ogmap.h>
#include <orcaogfusion/orcaogfusion.h>
#include <orcaice/orcaice.h>
#include "ogfusionI.h"
#include "handler.h"

using namespace std;

namespace ognode {

namespace {

    // Fuses the cell likelihoods into the map
    void add( const orca::OgCellLikelihood &f, orcaogmap::GenericMap<double> &m )
    {
        double prior;
        if( !m.tryGridCell( f.x, f.y, prior ) )
            return;

        double posterior = f.likelihood * prior;
        // normalise
        double normPosterior = posterior / (2.0*posterior + 1.0 - f.likelihood - prior );

        // cout<<"TRACE(handler.cpp): prior: " << prior << ", posterior: " << posterior << ", normPosterior: " << normPosterior << endl;

        CLIP_TO_LIMITS( ogfusion::ogLimitLowD, normPosterior, ogfusion::ogLimitHighD );

        m.gridCell( f.x, f.y ) = normPosterior;
    }

    void convert( const orcaogmap::GenericMap<double> &doubleMap, orca::OgMapData &ogMapData )
    {
        orcaogmap::OgMap local( doubleMap.numCellsX(),
                                doubleMap.numCellsY(),
                                doubleMap.offset(),
                                doubleMap.metresPerCellX(),
                                doubleMap.metresPerCellY(),
                                0 );

        for ( int xi=0; xi < local.numCellsX(); xi++ )
        {
            for ( int yi=0; yi < local.numCellsY(); yi++ )
            {
                local.gridCell( xi, yi ) = (unsigned char)floor( doubleMap.gridCell(xi,yi)*orcaogmap::CELL_OCCUPIED+0.5 );
            }
        }
        orcaogmap::convert( local, ogMapData );
    }

}

//////////////////////////////////////////////////////////////////////

Handler::Handler( const orcaice::Context &context )
    : ogFusionDataBuffer_(-1,hydroutil::BufferTypeCircular), // infinite depth
      context_(context)
{
}

Handler::~Handler()
{
}

void
Handler::init()
{
    while ( !isStopping() )
    {
        try {

            // get config
            Ice::PropertiesPtr prop = context_.properties();
            std::string prefix = context_.tag();
            prefix += ".Config.";

            // read map info from config
            ogfusion::MapConfig mapConfig;

            mapConfig.mapResX = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Map.ResX", 0.5 );
            mapConfig.mapResY = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Map.ResY", 0.5 );
    
            double sizeXMetres = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Map.SizeXMetres", 50.0 );
            double sizeYMetres = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Map.SizeYMetres", 50.0 );
    
            mapConfig.mapSizeX = (int)floor(sizeXMetres/mapConfig.mapResX);
            mapConfig.mapSizeY = (int)floor(sizeYMetres/mapConfig.mapResY);
            mapConfig.mapOriginX=orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Map.OriginX", -25.0 );
            mapConfig.mapOriginY=orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Map.OriginY", -25.0 );
            mapConfig.mapOrientation=orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"Map.Orientation", 0.0 );
            
            if ( !ogMapImpl_ )
            {
                // setup internal map
                internalMap_.reallocate( mapConfig.mapSizeX, 
                                         mapConfig.mapSizeY );
                internalMap_.offset().p.x = mapConfig.mapOriginX;
                internalMap_.offset().p.y = mapConfig.mapOriginY;
                internalMap_.offset().o   = mapConfig.mapOrientation;
                internalMap_.setMetresPerCellX( mapConfig.mapResX );
                internalMap_.setMetresPerCellY( mapConfig.mapResY );
                internalMap_.fill( 0.5 );

                orca::OgMapData initialOrcaMap;
                convert( internalMap_, initialOrcaMap );

                // set up ogmap interface
                ogMapImpl_ = new orcaifaceimpl::OgMapImpl( "OgMap", context_ );
                ogMapImpl_->initInterface( this );
                ogMapImpl_->localSetAndSend( initialOrcaMap );
            }

            if ( !ogFusionObjPtr_ )
            {
                // duplicate map config for orca::OgFusion Interface
                orca::OgFusionConfig ogFusionConfig;
                ogFusionConfig.offset.p.x=mapConfig.mapOriginX;
                ogFusionConfig.offset.p.y=mapConfig.mapOriginY;
                ogFusionConfig.offset.o=mapConfig.mapOrientation;
                ogFusionConfig.numCellsX=mapConfig.mapSizeX;
                ogFusionConfig.numCellsY=mapConfig.mapSizeY;
                ogFusionConfig.metresPerCellX=mapConfig.mapResX;
                ogFusionConfig.metresPerCellY=mapConfig.mapResY;

                // set up ogfusion interface
                ogFusionObjPtr_ = new OgFusionI( ogFusionConfig, ogFusionDataBuffer_ );
                orcaice::createInterfaceWithTag(context_, ogFusionObjPtr_, "OgFusion" );
            }

            // Enable network connections
            context_.activate();

            return;
        }
        catch ( const Ice::Exception & e )
        {
            stringstream ss;
            ss << "Handler: unexpected exception: " << e;
            context_.tracer()->error( ss.str() );
        }
        catch ( const std::exception & e )
        {
            stringstream ss;
            ss << "Handler: unexpected exception: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        catch ( ... )
        {
            context_.tracer()->error( "Handler: unexpected unknown exception.");
        }
    }
}

void
Handler::run()
{
    init();

    orca::OgFusionData data;
    orca::OgMapData map;
    
    while ( !isStopping() )
    {
        try
        {
            int ret=ogFusionDataBuffer_.getAndPopNext(data,1000);

            if(ret!=0) 
            {
                context_.tracer()->info("no ogfusion data available: waiting ...");
                continue;
            } 

            if ( data.observation.size() == 0 )
            {
                context_.tracer()->info("Received empty observation!");
                continue;
            }

            // cout << "Handler: got an object with " << data.observation.size() << " cells\n";

            for(unsigned int i=0;i<data.observation.size();i++)
            {
                add( data.observation[i], internalMap_ );
                // ogfusion::add(localMap_,data.observation[i]);
            }
            orca::OgMapData orcaOgMap;
            convert( internalMap_, orcaOgMap );
            orcaOgMap.timeStamp = data.timeStamp;

            // cout<<"TRACE(handler.cpp): orcaOgMap: " << orcaice::toString(orcaOgMap) << endl;
            

            ogMapImpl_->localSetAndSend( orcaOgMap );

        } // try
        catch ( const Ice::Exception & e )
        {
            stringstream ss;
            ss << "Handler: unexpected exception: " << e;
            context_.tracer()->error( ss.str() );
        }
        catch ( const std::exception & e )
        {
            stringstream ss;
            ss << "Handler: unexpected exception: " << e.what();
            context_.tracer()->error( ss.str() );
        }
        catch ( ... )
        {
            context_.tracer()->error( "Handler: unexpected unknown exception.");
        }
    } // end of main loop
    
    
    context_.tracer()->debug( "dropping out from run()", 5 );
    waitForStop();
}

}
