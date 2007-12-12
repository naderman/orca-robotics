/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include <sstream>
#include "sessionmanager.h"
#include <IceUtil/Time.h>
#include <orcaice/orcaice.h>

using namespace std;

namespace orcaicegrid {

SessionManager::SessionManager( SessionCreationCallback &sessionCreationCallback,
                                const orcaice::Context &context ) :
    SafeThread(context.tracer()),
    sessionCreationCallback_(sessionCreationCallback),
    timeoutSec_(0),
    context_(context)
      
{
}

bool
SessionManager::tryCreateSession()
{
    // config file parameters
    Ice::PropertiesPtr props = context_.properties();
    std::string prefix = context_.tag() + ".Config.";
   
    std::string instanceName = context_.properties()->getPropertyWithDefault( prefix+"IceGrid.InstanceName", "IceGrid" );
    IceGrid::RegistryPrx registry;

    try {
        string regName = instanceName+"/Registry";
        registry = IceGrid::RegistryPrx::checkedCast( context_.communicator()->stringToProxy(regName) );
        
        if(!registry)
        {
            context_.tracer().error( "SessionManager: Could not contact registry" );
            return false;
        }
    }
    catch ( const Ice::CommunicatorDestroyedException & )
    {
        // This is OK, we're shutting down
        return false;
    }
    catch ( const Ice::Exception &e )
    {
        std::stringstream ss;
        ss << "SessionManager: Error contacting registry: " << e;
        context_.tracer().error( ss.str() );
        return false;
    }
    catch ( const std::exception &e )
    {
        std::stringstream ss;
        ss << "SessionManager: Error contacting registry: " << e.what();
        context_.tracer().error( ss.str() );
        return false;
    }
    catch ( ... )
    {
        std::stringstream ss;
        ss << "SessionManager: Unknown error contacting registry.";
        context_.tracer().error( ss.str() );
        return false;
    }
            
    try
    {
        session_ = registry->createAdminSession( "sessionmanager.cpp-assume-no-access-control",
                                                 "sessionmanager.cpp-assume-no-access-control" );
        timeoutSec_ = registry->getSessionTimeout();
        stringstream ss; ss<<"SessionManager: Created session (timeout="<<timeoutSec_<<"s)";
        context_.tracer().info( ss.str() );

        bool success = sessionCreationCallback_.actionOnSessionCreation( session_ );
        if ( success )
        {
            context_.tracer().debug( "SessionManager: Created session." );
            return true;
        }
        else
        {
            context_.tracer().debug( "SessionManager: actionOnSessionCreation failed." );
            return false;
        }
    }
    catch( const std::exception& e)
    {
        stringstream ss;
        ss << "SessionManager: Error during actionOnSessionCreation: " << e.what();
        context_.tracer().error( ss.str() );
        return false;
    }
    catch( ... )
    {
        stringstream ss;
        ss << "SessionManager: Unknown exception during actionOnSessionCreation.";
        context_.tracer().error( ss.str() );
        return false;
    }
}

void
SessionManager::checkedSleep( int sec )
{
    for ( int i=0; i < sec; i++ )
    {
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(1));
        if ( !!isStopping() )
            break;
    }
}

void
SessionManager::walk()
{
    // This outer loop is repeated on session failure.
    while( !isStopping() )
    {
        try {
            // Create the session
            while ( !isStopping() )
            {
                if ( tryCreateSession() )
                    break;
                IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
            }

            // Keep it alive
            while ( !isStopping() )
            {
                checkedSleep( timeoutSec_/2 );

                try
                {
                    context_.tracer().debug( "SessionManager: sending keepAlive()" );
                    session_->keepAlive();
                }
                catch( const Ice::CommunicatorDestroyedException & )
                {
                    // This is OK, we're shutting down.
                    break;
                }
                catch( const Ice::Exception& e )
                {
                    stringstream ss; ss<<"SessionManager: Failed to keep session alive: "<<e;
                    context_.tracer().warning( ss.str() );
                    break;
                }
                catch( const std::exception& e )
                {
                    stringstream ss; ss<<"SessionManager: Failed to keep session alive: "<<e.what();
                    context_.tracer().warning( ss.str() );
                    break;
                }
            }
        }
        catch( const Ice::CommunicatorDestroyedException & )
        {
            // This is OK, we're shutting down.
        }
        catch ( std::exception &e )
        {
            stringstream ss;
            ss << "SessionManager: Caught stray exception: " << e.what();
            context_.tracer().warning( ss.str() );
        }
        catch ( ... )
        {
            context_.tracer().warning( "SessionManager: caught unknown stray exception." );
        }
    }

    try {
        // Destroying the session_ will release all allocated objects.
        context_.tracer().info( "SessionManager: Destroying session." );
        session_->destroy();
    }
    catch ( ... )
    {
        // Not much we can do about it...
    }
}

}
