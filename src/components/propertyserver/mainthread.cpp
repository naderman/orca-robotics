/*
 * Orca-Robotics Project: Components for robotics
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaifaceutil/properties.h>
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
    setMaxHeartbeatInterval( 20.0 );

    persistanceFile_ = orcaice::getPropertyWithDefault( context_.properties(),
                                                        context_.tag()+".Config.PersistanceFile",
                                                        "" );
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
                ss << "Received new properties: " << ifaceutil::toString( incomingProperties );
                context_.tracer().info( ss.str() );

                // Merge incoming properties with the local database.
                propertyDb_.addProperties( incomingProperties.properties );

                // Then inform the world
                orca::PropertiesData resultantProperties;
                resultantProperties.properties = propertyDb_.properties();
                propertiesInterface_->localSetAndSend( resultantProperties );
            }

            health().ok();
            continue;

        } // end of try
        catch ( ... )
        {
            orcaice::catchMainLoopExceptions( health() );
        }
    } // end of while

    //
    // Save our properties
    //
    if ( !persistanceFile_.empty() )
    {
        propertyDb_.writePropertiesToFile( persistanceFile_ );
    }
}

/////////////////////////

void
MainThread::initPropertiesDb()
{
    //
    // Read from the persistance file
    //

    if ( !persistanceFile_.empty() )
    {
        try {
            std::map<std::string,std::string> propsFromFile = hydropropertydb::readFromFile( persistanceFile_ );
            cout<<"Properties loaded from persistance file: " << hydropropertydb::toString(propsFromFile) << endl;
            propertyDb_.addProperties( propsFromFile );
        }
        catch ( std::exception &e )
        {
            health().fault( e.what() );
        }
    }

    //
    // Read from the config file
    //

    // Only insert properties with this prefix (which gets stripped)
    std::string prefix = context_.tag()+".Config.InitProperty.";
    map<string,string> props = context_.properties()->getPropertiesForPrefix(prefix);

    map<string,string> strippedProps;
    for ( map<string,string>::const_iterator it=props.begin(); it!=props.end(); ++it )
    {
        strippedProps.insert( make_pair(it->first.substr(prefix.size()),it->second) );
    }
    cout << "Properties loaded from config file: " << hydropropertydb::toString(strippedProps) << endl;

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
