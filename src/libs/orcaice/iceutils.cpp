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

#include "iceutils.h"
#include "configutils.h"
#include "exceptions.h"

using namespace std;

namespace orcaice {

void
createInterfaceWithString( const Context      & context,
                            Ice::ObjectPtr    & object,
                            const std::string & proxyString )
{
    try
    {
        // register object with the adapter
        context.adapter()->add( object, context.communicator()->stringToIdentity(proxyString) );
    }
    catch ( const Ice::AlreadyRegisteredException &e )
    {
        std::stringstream ss;
        ss<<"Failed to create interface "<<proxyString<<": "<<e;
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }
    catch( const Ice::ObjectAdapterDeactivatedException &e )
    {
        std::stringstream ss;
        ss << "orcaice::Component: Failed to create interface because the adapter is destroyed : " << e;
        context.tracer().warning( ss.str() );
        throw orcaice::ComponentDeactivatingException( ERROR_INFO, ss.str() );
    }

    // locally register this interface with Home interface 
    orca::ProvidedInterface iface;
    iface.name = proxyString;
    // this is a local call
    iface.id   = object->ice_id();
    context.home().addProvidedInterface( iface );
}

void
createInterfaceWithTag( const Context      & context,
                        Ice::ObjectPtr     & object,
                        const std::string  & ifaceTag )
{
    // look up naming information in the properties
    orca::FQInterfaceName fqIName = getProvidedInterface( context, ifaceTag );

    createInterfaceWithString( context, object, fqIName.iface );
}


void 
createInterfaceWithString( const Context       & context,
                           Ice::ObjectPtr      & object,
                           const std::string   & name,
                           gbxiceutilacfr::Thread*  thread, const std::string& subsysName, 
                           int retryInterval, int retryNumber )
{
    int count = 0;
    while ( !thread->isStopping() && ( retryNumber<0 || count<retryNumber) )
    {
        try {
            createInterfaceWithString( context, object, name );
            break;
        }
        catch ( const std::exception& e ) {
            std::stringstream ss;
            ss << "Failed to create interface with string "<<name<<":\n"
               << e.what() << endl
               <<"Will retry in "<<retryInterval<<"s.\n";
            context.tracer().warning( ss.str() );
        }
        catch ( ... ) {
            std::stringstream ss;
            ss << "Caught something while creating interface with string "<<name<<". "
                <<"Will retry in "<<retryInterval<<"s.\n";
            context.tracer().warning( ss.str() );
        }
        ++count;
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(retryInterval));
        if ( !subsysName.empty() ) {
            context.status().heartbeat( subsysName );
        }
    }
}

void 
createInterfaceWithTag( const Context       & context,
                        Ice::ObjectPtr      & object,
                        const std::string   & interfaceTag,
                            gbxiceutilacfr::Thread*  thread, const std::string& subsysName, 
                            int retryInterval, int retryNumber )
{
    int count = 0;
    while ( !thread->isStopping() && ( retryNumber<0 || count<retryNumber) )
    {
        try {
            createInterfaceWithTag( context, object, interfaceTag );
            break;
        }
        catch ( const std::exception& e ) {
            std::stringstream ss;
            ss << "Failed to create interface with tag "<<interfaceTag<<":\n"
               << e.what() << endl
               <<"Will retry in "<<retryInterval<<"s.";
            context.tracer().warning( ss.str() );
        }
        catch ( ... ) {
            std::stringstream ss;
            ss << "Caught something while creating interface with tag "<<interfaceTag<<". "
                <<"Will retry in "<<retryInterval<<"s.\n";
            context.tracer().warning( ss.str() );
        }
        ++count;
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(retryInterval));
        if ( !subsysName.empty() ) {
            context.status().heartbeat( subsysName );
        }
    }
}

void
activate( Context& context, 
            gbxiceutilacfr::Thread*  thread, const std::string& subsysName, 
            int retryInterval, int retryNumber )
{
    int count = 0;
    while ( !thread->isStopping() && ( retryNumber<0 || count<retryNumber) )
    {
        try {
            context.activate();
            break;
        }
        // alexm: in all of these exception handlers we would like to just catch
        // std::exception but Ice 3.2 does not overload e.what() so we have to do it
        // separately.
        catch ( Ice::CommunicatorDestroyedException )
        {
            // This means we're shutting down.
            break;
        }
        catch ( const std::exception& e ) {
            std::stringstream ss;
            ss << "Failed to activate component:\n"
               <<e.what()<<endl
               <<"Will retry in "<<retryInterval<<"s.\n";
            context.tracer().warning( ss.str() );
        }
        catch ( ... ) {
            std::stringstream ss;
            ss << "Caught something while activating. "
                <<"Will retry in "<<retryInterval<<"s.\n";
            context.tracer().warning( ss.str() );
        }
        ++count;
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(retryInterval));
        if ( !subsysName.empty() ) {
            context.status().heartbeat( subsysName );
        }
    }
}

void
tryRemoveInterface( orcaice::Context& context, const std::string& interfaceName )
{
    if ( !context.communicator() ) {
        // the communicator is already destroyed.
        return;
    }

    const Ice::Identity id = context.communicator()->stringToIdentity( interfaceName );
    tryRemoveInterfaceWithIdentity( context, id );
}

void 
tryRemoveInterfaceWithIdentity( orcaice::Context& context, const Ice::Identity& interfaceId )
{
    if ( !context.communicator() ) {
        // the communicator is already destroyed.
        return;
    }

    try {
        context.adapter()->remove( interfaceId );
        stringstream ss;
        ss << "Removed Ice::ObjectPtr with identity "<<context.communicator()->identityToString(interfaceId)<<" from adapter.";
        context.tracer().debug( ss.str() );
    }
    catch ( Ice::ObjectAdapterDeactivatedException& )
    {
        // the communicator is already shut down.
    }
    catch ( std::exception &e )
    {
        stringstream ss;
        ss << "(while removing Ice::ObjectPtr with identity "<<context.communicator()->identityToString(interfaceId)<<" from adapter): " << e.what();
        context.tracer().warning( ss.str() );
    }
}

} // namespace
