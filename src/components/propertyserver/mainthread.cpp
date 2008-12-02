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

namespace propserver {

MainThread::MainThread( const orcaice::Context &context ) :
    orcaice::SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    propertiesInterface_( new orcaifaceimpl::PropertiesImpl( "Properties", context ) ),
    context_(context)
{
}

void 
MainThread::initialise()
{
    subStatus().setMaxHeartbeatInterval( 20.0 );


    // These functions catch their exceptions.
    activate( context_, this, subsysName() );

    initPropertiesDb();
    initNetworkInterface();
}

void
MainThread::work()
{
    orca::PropertiesData incomingProperties;

    while ( !isStopping() )
    {
        try 
        {
            // this blocks until new data arrives
            const int TIMEOUT_MS = 1000;
            int ret = propertiesInterface_->localGetRemotelySetDataStore().getNext( incomingProperties,
                                                                                    TIMEOUT_MS );
            if ( ret == 0 )
            {
                stringstream ss;
                ss << "Received new properties: " << orcaobj::toString( incomingProperties );
                context_.tracer().info( ss.str() );

                // Set our local database.
                propertyDb_.addProperties( incomingProperties.properties );

                // Then inform the world
                orca::PropertiesData propData;
                propData.properties = propertyDb_.properties();
                propertiesInterface_->localSetAndSend( propData );
            }

            subStatus().ok();
            continue;

        } // end of try
        catch ( ... ) 
        {
            orcaice::catchMainLoopExceptions( subStatus() );
        }
    } // end of while
}

/////////////////////////

void
MainThread::initPropertiesDb()
{
    // Only insert properties with this prefix (which gets stripped)
    std::string prefix = context_.tag()+".Config.";
    map<string,string> props = context_.properties()->getPropertiesForPrefix(prefix);

    map<string,string> strippedProps;
    for ( map<string,string>::const_iterator it=props.begin(); it!=props.end(); ++it ) 
    {
        strippedProps.insert( make_pair(it->first.substr(prefix.size()),it->second) );
    }

    stringstream ss;
    ss << "MainThread: Properties loaded from file: " << toString(strippedProps);
    context_.tracer().debug( ss.str() );

    propertyDb_.addProperties( strippedProps );
}

void
MainThread::initNetworkInterface()
{
    // set the interface up with the contents of the property database
    orca::PropertiesData pData;
    pData.properties = propertyDb_.properties();
    propertiesInterface_->localSet( pData );
    propertiesInterface_->initInterface( this, subsysName() );
    propertiesInterface_->localSetAndSend( pData );
}

}
