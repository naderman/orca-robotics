/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2006-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <sstream>
#include "mainloop.h"
#include <IceUtil/Time.h>
#include <orcaice/orcaice.h>

using namespace std;
using namespace icegridmon;


MainLoop::MainLoop( const orcaice::Context &context )
    : context_(context)
{
    // Create all observers
    registryObserver_ = new RegistryObserverI( context_ );
    Ice::ObjectPtr registryObj = registryObserver_;
    registryObserverPrx_ =
        orcaice::createConsumerInterface<IceGrid::RegistryObserverPrx> ( context_, registryObj );

    applicationObserver_ = new ApplicationObserverI( context_ );
    Ice::ObjectPtr applicationObj = applicationObserver_;
    applicationObserverPrx_ =
        orcaice::createConsumerInterface<IceGrid::ApplicationObserverPrx> ( context_, applicationObj );

    adapterObserver_ = new AdapterObserverI( context_ );
    Ice::ObjectPtr adapterObj = adapterObserver_;
    adapterObserverPrx_ =
        orcaice::createConsumerInterface<IceGrid::AdapterObserverPrx> ( context_, adapterObj );

    objectObserver_ = new ObjectObserverI( context_ );
    Ice::ObjectPtr objectObj = objectObserver_;
    objectObserverPrx_ =
        orcaice::createConsumerInterface<IceGrid::ObjectObserverPrx> ( context_, objectObj );

    nodeObserver_ = new NodeObserverI( context_ );
    Ice::ObjectPtr nodeObj = nodeObserver_;
    nodeObserverPrx_ =
        orcaice::createConsumerInterface<IceGrid::NodeObserverPrx> ( context_, nodeObj );    
}

void
MainLoop::activate()
{
    while ( isActive() )
    {
        try {
            context_.activate();
            return;
        }
        catch ( std::exception & e )
        {
            std::stringstream ss;
            ss << "MainLoop::activate(): Caught exception: " << e.what();
            context_.tracer()->warning( ss.str() );
        }
        catch ( ... )
        {
            context_.tracer()->warning( "MainLoop::activate(): caught unknown exception." );
        }
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
    }
}

bool
MainLoop::tryCreateSession()
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
            context_.tracer()->error( "MainLoop: Could not contact registry" );
            return false;
        }
    }
//     catch ( const Ice::Exception &e )
//     {
//         std::stringstream ss;
//         ss << "MainLoop: Error contacting registry: " << e;
//         context_.tracer()->error( ss.str() );
//         return false;
//     }
    catch ( const std::exception &e )
    {
        std::stringstream ss;
        ss << "MainLoop: Error contacting registry: " << e.what();
        context_.tracer()->error( ss.str() );
        return false;
    }
    catch ( ... )
    {
        std::stringstream ss;
        ss << "MainLoop: Unknown error contacting registry.";
        context_.tracer()->error( ss.str() );
        return false;
    }
            
    int timeoutSec = 0;
    try
    {
        session_ = registry->createAdminSession( "assume-no-access-control", "assume-no-access-control" );
        timeoutSec = registry->getSessionTimeout();
        timeout_ = IceUtil::Time::seconds(timeoutSec);
        
        stringstream ss; ss<<"Created session (timeout="<<timeoutSec<<"s";
        context_.tracer()->info( ss.str() );
    }
    catch( const std::exception& e)
    {
        stringstream ss;
        ss << "MainLoop: Error creating Admin Session: " << e.what();
        context_.tracer()->error( ss.str() );
        return false;
    }

    try {
        session_->setObservers( registryObserverPrx_, 
                                nodeObserverPrx_,
                                applicationObserverPrx_,
                                adapterObserverPrx_,
                                objectObserverPrx_ );
    }
    catch ( const std::exception &e )
    {
        stringstream ss;
        ss << "MainLoop: Failed to register observers: " << e.what();
        context_.tracer()->error( ss.str() );
        return false;
    }

    context_.tracer()->debug( "MainLoop: Created session and registered observers." );
    return true;
}

void
MainLoop::run()
{
    activate();

    // This outer loop is repeated on session failure.
    while( isActive() )
    {
        // Create the session
        while ( isActive() )
        {
            if ( tryCreateSession() )
                break;
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
        }

        // Keep it alive
        while ( isActive() )
        {
            IceUtil::ThreadControl::sleep(timeout_);

            try
            {
                context_.tracer()->debug( "MainLoop: sending keepAlive()" );
                session_->keepAlive();
            }
            catch( const std::exception& e )
            {
                stringstream ss; ss<<"Failed to keep session alive: "<<e.what();
                context_.tracer()->warning( ss.str() );
                break;
            }
        }
    }

    // Destroying the session_ will release all allocated objects.
    context_.tracer()->info( "MainLoop: Destroying session." );
    session_->destroy();
}
