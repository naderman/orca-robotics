/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "componentthread.h"
#include <orcaice/exceptions.h>
#include <orcaice/catchutils.h>
#include <orcaice/stringutils.h>
#include <iostream>
#include <IceGrid/Registry.h>  // used to register Home interface as a well-known object

using namespace std;
using namespace orcaice;

ComponentThread::ComponentThread( const orcaice::Context& context ) :
    SafeThread(context.tracer()),
    context_(context)
{
}

void
ComponentThread::walk()
{    
    context_.status().infrastructureInitialising();

    Ice::PropertiesPtr props = context_.properties();
    bool hasStatusInterface = props->getPropertyAsInt( "Orca.Component.EnableStatus" );
    bool hasHomeInterface = props->getPropertyAsInt( "Orca.Component.EnableHome" );
    
    cout<<"DEBUG: hasStatusInterface="<<hasStatusInterface<<" hasHomeInterface="<<hasHomeInterface<<endl;

    bool registeredHome = false;

    const int sleepIntervalMs = 1000;

    context_.status().infrastructureWorking();

    try {
        while ( !isStopping() )
        {
            bool needToRegisterHome = hasHomeInterface && !registeredHome;

            if ( !needToRegisterHome && !hasStatusInterface )
            {
                context_.tracer().info( "ComponentThread: Nothing left to do, so quitting" );
                return;
            }

            if ( !registeredHome && hasHomeInterface )
            {
                registeredHome = tryRegisterHome();
            }

            if ( hasStatusInterface )
            {
                context_.status().process();
            }

            IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(sleepIntervalMs));
        }
    }
    catch ( ... )
    {
        orcaice::catchExceptions( context_.tracer(), "running component utility thread" );
    }

    context_.status().infrastructureFinalising();
}

//
// PROVIDED INTERFACE: Home
// Make Home a well-known object, by adding it to the registry
//
bool
ComponentThread::tryRegisterHome()
{
    std::string homeIdentityString = toHomeIdentity( context_.name() );
    Ice::Identity homeIdentity = context_.communicator()->stringToIdentity(homeIdentityString);

    // previous method: adding Home to the component adapter
//     Ice::ObjectPrx homePrx = context_.adapter()->createProxy( homeIdentity );

    // EXPERIMENTAL! adding Home as a facet to the Admin interface.
    Ice::ObjectPrx adminPrx = context_.communicator()->getAdmin();
    assert( adminPrx && "Null admin proxy when registering home" );
    cout<<"DEBUG: admin proxy: "<<adminPrx->ice_toString()<<endl;

    // change generic proxy to the Home facet.
    Ice::ObjectPrx homePrx = adminPrx->ice_facet( "Home" );

    // apparently cannot change just the registered name. it changes the proxy, which is wrong.
//     homePrx = homePrx->ice_identity( homeIdentity );
//     cout<<"DEBUG: home proxy: "<<homePrx->ice_toString()<<endl;

//     context_.tracer().info( string("Registering Home with identity ")+homeIdentityString );

    std::string instanceName = context_.properties()->getPropertyWithDefault( "IceGrid.InstanceName", "IceGrid" );
    Ice::ObjectPrx base = context_.communicator()->stringToProxy( instanceName+"/Registry" );
    try {
        // Open an admin session with the registry
        IceGrid::RegistryPrx registry = IceGrid::RegistryPrx::checkedCast(base);
        // This assumes no access control
        std::string username = "componentthread-no-access-control";
        std::string password = "componentthread-no-access-control";
        IceGrid::AdminSessionPrx adminSession = registry->createAdminSession( username, password );

        //
        // use the adminSession to add our Home interface
        //
        IceGrid::AdminPrx admin = adminSession->getAdmin();
        try {
            admin->addObjectWithType( homePrx, "::orca::Home" );
        }
        catch ( const IceGrid::ObjectExistsException& ) {
            admin->updateObject( homePrx );
        }
        context_.tracer().info( string("Registered Home as: ")+homePrx->ice_toString() );

        // we don't need the session anymore
        // (we can just leave it there and it would be destroyed eventually without being kept alive, but it's
        // more transparent if we destroy it ourselves)
        adminSession->destroy();
    }
    catch ( Ice::CommunicatorDestroyedException& ) 
    {
        // Ignore -- we're shutting down.
    }
    catch ( Ice::Exception& e ) 
    {
        bool requireRegistry = context_.properties()->getPropertyAsInt( "Orca.Component.RequireRegistry" );
        if ( requireRegistry ) {
            std::stringstream ss;
            ss << "Failed to register Home interface: "<<e<<".  Check IceGrid Registry.  You may allow things to continue without registration by setting Orca.RequireRegistry=0.";
            context_.tracer().error( ss.str() );
            return false;
        }
        else {
            std::stringstream ss;
            ss << "Failed to register Home interface: "<<e<<".";
            context_.tracer().warning( ss.str() );
            context_.tracer().info( "You may enforce registration by setting Orca.RequireRegistry=1." );
            // TODO: this is misleading! we haven't registered.
            return true;
        }
    }
    return true;
}
