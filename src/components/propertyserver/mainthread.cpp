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

namespace propserver {

MainThread::MainThread( const orcaice::Context &context ) :
    hydroiceutil::SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    propertiesInterface_( new orcaifaceimpl::PropertiesImpl( "Properties", context ) ),
    context_(context)
{
    subStatus().setMaxHeartbeatInterval( 20.0 );
}

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
    // set the interface up with the context of the property database
    orca::PropertiesData pData;
    pData.properties = propertyDb_.properties();
    propertiesInterface_->localSet( pData );

    propertiesInterface_->initInterface( this, subsysName() );
}

void
MainThread::walk()
{
    stringstream exceptionSS;

    orca::PropertiesData incomingProperties;

    // These functions catch their exceptions.
    activate( context_, this, subsysName() );

    initPropertiesDb();
    initNetworkInterface();

    //
    // IMPORTANT: Have to keep this loop rolling, because the '!isStopping()' call checks for requests to shut down.
    //            So we have to avoid getting stuck anywhere within this main loop.
    //
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
                ss << "Received new properties: " << orcaice::toString( incomingProperties );
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
        catch ( Ice::CommunicatorDestroyedException & ) {
            // This is OK: it means that the communicator shut down (eg via Ctrl-C)
            // somewhere in mainLoop. Eventually, component will tell us to stop.
        }
        catch ( const Ice::Exception &e ) {
            exceptionSS << "ERROR(mainthread.cpp): Caught unexpected exception: " << e;
        }
        catch ( const std::exception &e ) {
            exceptionSS << "ERROR(mainthread.cpp): Caught unexpected exception: " << e.what();
        }
        catch ( const std::string &e ) {
            exceptionSS << "ERROR(mainthread.cpp): Caught unexpected string: " << e;
        }
        catch ( const char *e ) {
            exceptionSS << "ERROR(mainthread.cpp): Caught unexpected char *: " << e;
        }
        catch ( ... ) {
            exceptionSS << "ERROR(mainthread.cpp): Caught unexpected unknown exception.";
        }

        if ( !exceptionSS.str().empty() ) {
            context_.tracer().error( exceptionSS.str() );
            subStatus().fault( exceptionSS.str() );     
            exceptionSS.str("");
        }
    } // end of while
}

}
