/*
 * Orca-Robotics Project: Components for robotics
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "componentthread.h"
#include <orcaice/exceptions.h>
#include <orcaice/catchutils.h>
#include <orcaice/icegridutils.h>
#include <orcaice/multiiceutils.h> // for activate()
#include "privateutils.h"
#include <iostream>
#include <Glacier2/Glacier2.h>
#include <IceGrid/Registry.h>  // used to register Home interface as a well-known object
#include <gbxsickacfr/gbxiceutilacfr/timer.h>

using namespace std;

namespace orcaice {
namespace detail {

ComponentThread::ComponentThread( ComponentAdapterActivationPolicy adapterPolicy,
                                  const orcaice::Context& context ) :
    SafeThread(context.tracer()),
    adapterPolicy_(adapterPolicy),
    context_(context)
{
}

void
ComponentThread::walk()
{
    // change state
    context_.status().infrastructureInitialising();

    Ice::PropertiesPtr props = context_.properties();

    bool hasStatusInterface = props->getPropertyAsInt( "Orca.Component.EnableStatus" );

    bool hasHomeInterface = props->getPropertyAsInt( "Orca.Component.EnableHome" );

    bool needPokeHistory = context_.history().isEnabled() && context_.history().autoSaveInterval()>0;

    bool needPokeRouter = ( context_.communicator()->getDefaultRouter()!=0 );

//     cout<<"DEBUG: hasStatusInterface="<<hasStatusInterface<<" hasHomeInterface="<<hasHomeInterface<<endl;

    bool registeredHome = false;

    const int sleepIntervalMs = 1000;

    // change state
    context_.status().infrastructureWorking();

    //
    // connect to the router, if configured
    // this needs to happen first, in case we are using the Registry behind the firewall
    // TODO: try multiple times
    //
    if ( needPokeRouter ) {
        context_.tracer().info( "Component infrastructure: connecting to router..." );
        Ice::RouterPrx defaultRouter = context_.communicator()->getDefaultRouter();
        Glacier2::RouterPrx router;
        try
        {
            router = Glacier2::RouterPrx::checkedCast(defaultRouter);
        }
        catch ( const Ice::Exception& e ) {
            context_.tracer().error( "checked cast failed:\n" + string(e.what()) );
        }

        try
        {
            routerSession_ = router->createSession("routersession-no-access-control",
                                                   "routersession-no-access-control");
            sessionTimeoutSec_ = router->getSessionTimeout();
            stringstream ss;
            ss<<"Component infrastructure: connected to router with session timeout="<<sessionTimeoutSec_<<"sec";
            context_.tracer().info( ss.str() );
            assert( routerSession_ );
            routerSession_->ice_ping();
        }
        catch(const Glacier2::PermissionDeniedException& ex) {
            context_.tracer().error( "permission denied:\n" + ex.reason );
        }
        catch(const Glacier2::CannotCreateSessionException& ex) {
            context_.tracer().error( "cannot create session:\n" + ex.reason );
        }

        keepAliveTimer_ = new gbxiceutilacfr::Timer();
    }

    //
    // activate component's adapter
    //
    if ( adapterPolicy_ == AdapterAutoActivation ) {
        // not supplying subsystem name because we are in a special Infrastructure subsystem.
        activateAdapter( context_, this );
        context_.tracer().info( "Component infrastructure: adapter activated." );
    }

    try {
        while ( !isStopping() )
        {
            // update dynamic flag
            bool needRegisterHome = hasHomeInterface && !registeredHome;

            // check that we still have something to do
            if ( !needRegisterHome &&
                 !hasStatusInterface &&
                 !needPokeHistory &&
                 !needPokeRouter)
            {
                context_.tracer().info( "Component infrastructure: nothing left to do, quitting" );
                return;
            }

            if ( !registeredHome && hasHomeInterface )
                registeredHome = tryRegisterHome();

            if ( hasStatusInterface )
                context_.status().process();

            if ( needPokeHistory )
                context_.history().report();

            if ( needPokeRouter && keepAliveTimer_->elapsedSec()>sessionTimeoutSec_/4 )
                tryKeepRouterSessionAlive();

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
    try {
        detail::registerHomeInterface( context_ );
    }
    catch ( Ice::Exception& e )
    {
        bool requireRegistry = context_.properties()->getPropertyAsInt( "Orca.Component.RequireRegistry" );
        if ( requireRegistry ) {
            std::stringstream ss;
            ss << "Failed to register Home interface: "<<e<<".  Check IceGrid Registry.  You may allow things to continue without registration by setting Orca.Component.RequireRegistry=0.";
            context_.tracer().error( ss.str() );
            return false;
        }
        else {
            std::stringstream ss;
            ss << "Failed to register Home interface: "<<e<<".";
            context_.tracer().warning( ss.str() );
            context_.tracer().info( "You may enforce registration by setting Orca.Component.RequireRegistry=1." );
            // TODO: returning TRUE is misleading! we haven't registered.
            return true;
        }
    }
    return true;
}

void
ComponentThread::tryKeepRouterSessionAlive()
{
    try
    {
        routerSession_->ice_ping();

        keepAliveTimer_->restart();
    }
    catch( const Ice::CommunicatorDestroyedException & ) {
        // This is OK, we're shutting down.
    }
    catch( const std::exception& e ) {
        context_.tracer().warning( "IceGridSession: Failed to keep session alive: "+string(e.what()) );
        // TODO: need to reconnect to the router
    }
}

} // detail namespace
} // orcaice namespace
