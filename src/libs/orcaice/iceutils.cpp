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
#include <Ice/Ice.h>

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
        context.tracer().debug( ss.str(),6 );
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
