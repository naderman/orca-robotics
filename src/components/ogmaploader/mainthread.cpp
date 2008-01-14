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
#include "fakemaploader.h"
#include "maploader.h"

using namespace std;
using namespace ogmaploader;

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
    orca::OgMapData theMap;

    std::string driverName = orcaice::getPropertyWithDefault( prop, prefix+"Driver", "fake" );
    if ( driverName == "fake" )
    {
        cout<<"TRACE(maploadercomponent.cpp): Instantiating fake driver" << endl;
        fakeLoadMap( theMap );
    }
    else if ( driverName == "file" )
    {
        loadMapFromFile( context_,theMap );
        cout<<"TRACE(component.cpp): Loaded map: " << orcaice::toString(theMap) << endl;
    }
    else
    {
        // unrecoverable error
        context_.shutdown();
        throw hydroutil::Exception( ERROR_INFO, "Unknown driver type: "+driverName );
    }

    //
    // EXTERNAL PROVIDED INTERFACES
    //
    ogMapImpl_ = new orcaifaceimpl::OgMapImpl( "OgMap", context_ );
    ogMapImpl_->initInterface( this, subsysName() );
    ogMapImpl_->localSetAndSend( theMap );

    //
    // ENABLE NETWORK CONNECTIONS
    //
    // multi-try function
    orcaice::activate( context_, this, subsysName() );

    // init subsystem is done and is about to terminate
    subStatus().ok( "Initialized." );
    subStatus().setMaxHeartbeatInterval( -1 );
}
