#include "util.h"
#include <iostream>

using namespace std;

namespace orcaifaceimpl {

void
tryRemovePtr( orcaice::Context &context,
              Ice::ObjectPtr &ptr )
{
    if ( ptr == 0 ) return;

    try {
        context.adapter()->remove( context.communicator()->stringToIdentity( ptr->ice_id() ) );
    }
    catch ( Ice::ObjectAdapterDeactivatedException &e )
    {
        // This is OK, we're shutting down.
    }
    catch ( Ice::Exception &e )
    {
        cout << "orcaifaceimpl: Exception when removing Ice::ObjectPtr from adapter: " << e << endl;
    }
}

}
