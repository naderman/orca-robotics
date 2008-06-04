/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaobj/orcaobj.h>

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

MainThread::MainThread( const orcaice::Context &context ) : 
    SubsystemThread( context.tracer(), context.status(), "MainThread" ),
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
            orcaobj::loadFromFile( mapFileName, theMap );
        }
        catch ( const gbxutilacfr::Exception& e )
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
        throw gbxutilacfr::Exception( ERROR_INFO, s );
    }

    stringstream ssDebug;
    ssDebug << "Loaded map: " << endl << orcaobj::toString(theMap);
    context_.tracer().debug( ssDebug.str(), 2 );

    //
    // EXTERNAL PROVIDED INTERFACES
    //
    // create servant for direct connections
    featureMap2dImpl_ = new orcaifaceimpl::FeatureMap2dImpl( "FeatureMap2d", context_ );
    featureMap2dImpl_->initInterface( this, subsysName() );
    featureMap2dImpl_->localSetAndSend( theMap );

    //
    // ENABLE NETWORK CONNECTIONS
    //
    // multi-try function
    orcaice::activate( context_, this, subsysName() );

    // init subsystem is done and is about to terminate
    subStatus().ok( "Initialized." );
    subStatus().setMaxHeartbeatInterval( -1 );
}
