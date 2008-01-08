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
#include <orcaice/orcaice.h>

#include "mainthread.h"

using namespace std;
using namespace featuremaploader;

namespace {

void fakeLoadMap( orca::FeatureMap2dData &theMap )
{
    theMap.features.clear();

    orca::CartesianPointFeature2dPtr f1 = new orca::CartesianPointFeature2d;
    f1->type = orca::feature::LASERREFLECTOR;
    f1->p.x  = 0.0;
    f1->p.y  = 0.0;
    theMap.features.push_back( f1 );

    orca::CartesianPointFeature2dPtr f2 = new orca::CartesianPointFeature2d;
    f2->type = orca::feature::LASERREFLECTOR;
    f2->p.x  = 20.0;
    f2->p.y  = 20.0;
    theMap.features.push_back( f2 );
}

}

MainThread::MainThread( const orcaifaceimpl::FeatureMap2dImplPtr& obj, 
                const orcaice::Context &context ) : 
    SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    obj_(obj),
    context_(context)
{
    subStatus().setMaxHeartbeatInterval( 10.0 );
}

void
MainThread::walk()
{
    //
    // INITIAL CONFIGURATION
    //
    Ice::PropertiesPtr prop = context_.properties();
    std::string prefix = context_.tag() + ".Config.";

    //
    // LOAD THE MAP
    //
    orca::FeatureMap2dData theMap;

    std::string driverName = orcaice::getPropertyWithDefault( prop, prefix+"Driver", "fake" );
    if ( driverName == "fake" )
    {
        cout<<"TRACE(component.cpp): Instantiating fake driver" << endl;
        fakeLoadMap( theMap );
    }
    else if ( driverName == "real" )
    {
        std::string mapFileName = orcaice::getPropertyWithDefault( prop, prefix+"MapFileName", "featuremap.txt" );
        try
        {
            orcaice::loadFromFile( mapFileName, theMap );
        }
        catch ( const hydroutil::Exception& e )
        {   
            // unrecoverable error
            context_.shutdown();
            throw;
        }
    }
    else
    {
        // unrecoverable error
        context_.shutdown();

        std::string s = "Unknown driver type: "+driverName;
        throw hydroutil::Exception( ERROR_INFO, s );
    }

    obj_->initInterface( this, subsysName() );
    obj_->localSetAndSend( theMap );

    //
    // ENABLE NETWORK CONNECTIONS
    //
    // multi-try function
    orcaice::activate( context_, this, subsysName() );

    // init subsystem is done and is about to terminate
    subStatus().ok( "Initialized." );
    subStatus().setMaxHeartbeatInterval( -1 );
}
