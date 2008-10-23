#include "util.h"
#include <iostream>
#include <sstream>
#include <orcaice/orcaice.h>

using namespace std;

namespace orcaifaceimpl {

void
tryRemoveInterface( orcaice::Context &context,
                    const std::string &interfaceName )
{
    if ( !context.communicator() ) {
        // the communicator is already destroyed.
        return;
    }

    const Ice::Identity id = context.communicator()->stringToIdentity( interfaceName );
    tryRemoveInterfaceWithIdentity( context, id );
}

void 
tryRemoveInterfaceWithIdentity( orcaice::Context  &context,
                            const Ice::Identity &interfaceId )
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
    catch ( Ice::ObjectAdapterDeactivatedException & )
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

std::string getInterfaceNameFromTag( const orcaice::Context &context,
                                     const std::string      &interfaceTag )
{
    orca::FQInterfaceName fqIName = orcaice::getProvidedInterface( context, interfaceTag );
    return fqIName.iface;
}
    
std::string getTopicNameFromInterfaceName( const orcaice::Context &context,
                                           const std::string      &interfaceName )
{
    return orcaice::toString( orcaice::getProvidedTopicWithString( context, interfaceName ) );
}

}
