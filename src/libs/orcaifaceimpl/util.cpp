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
        // the communicator is probably already destroyed.
        return;
    }

    try {
        context.adapter()->remove( context.communicator()->stringToIdentity( interfaceName ) );
    }
    catch ( Ice::ObjectAdapterDeactivatedException & )
    {
        // This is OK, we're shutting down.
    }
    catch ( Ice::Exception &e )
    {
        stringstream ss;
        ss << "Caught orcaifaceimpl: Exception when removing Ice::ObjectPtr from adapter: " << e;
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
