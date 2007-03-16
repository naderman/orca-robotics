/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2006-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <sstream>

#include <IceGrid/Registry.h>
#include <IceGrid/Observer.h>
#include <orcaice/orcaice.h>
#include <orcalog/orcalog.h>

#include "component.h"

using namespace std;

namespace icegridmon {

Component::Component( const std::string & compName )
    : orcaice::Component( compName )
{
}

Component::~Component()
{
}

void 
Component::start()
{
    // config file parameters
    Ice::PropertiesPtr props = properties();
    std::string prefix = tag() + ".Config.";
   
    // activate component, this may throw and it will kill us
    activate();

    std::string instanceName = properties()->getPropertyWithDefault( "IceGrid.InstanceName", "IceGrid" );

    IceGrid::RegistryPrx registry = 
        IceGrid::RegistryPrx::checkedCast( context().communicator()->stringToProxy(instanceName+"/Registry") );

    if(!registry)
    {
        tracer()->error( "Could not contact registry" );
        context().communicator()->shutdown();
    }

    int timeoutMs = 0;
    try
    {
        session_ = registry->createAdminSession( "assume-no-access-control", "assume-no-access-control" );
        timeoutMs = registry->getSessionTimeout();
        
        stringstream ss; ss<<"Created session (timeout="<<timeoutMs<<"ms";
        tracer()->info( ss.str() );
    }
    catch(const IceGrid::PermissionDeniedException& ex)
    {
        tracer()->error( "Permission denied:\n" + ex.reason );
        throw;
    }

    sessionKeepAlive_ = new SessionKeepAliveThread( session_, timeoutMs/2, context() );
    sessionKeepAlive_->start();

    registryObserver_ = new RegistryObserverI( context() );
    Ice::ObjectPtr registryObj = registryObserver_;
    IceGrid::RegistryObserverPrx registryObserverPrx =
        orcaice::createConsumerInterface<IceGrid::RegistryObserverPrx> ( context(), registryObj );

    applicationObserver_ = new ApplicationObserverI( context() );
    Ice::ObjectPtr applicationObj = applicationObserver_;
    IceGrid::ApplicationObserverPrx applicationObserverPrx =
        orcaice::createConsumerInterface<IceGrid::ApplicationObserverPrx> ( context(), applicationObj );

    adapterObserver_ = new AdapterObserverI( context() );
    Ice::ObjectPtr adapterObj = adapterObserver_;
    IceGrid::AdapterObserverPrx adapterObserverPrx =
        orcaice::createConsumerInterface<IceGrid::AdapterObserverPrx> ( context(), adapterObj );

    objectObserver_ = new ObjectObserverI( context() );
    Ice::ObjectPtr objectObj = objectObserver_;
    IceGrid::ObjectObserverPrx objectObserverPrx =
        orcaice::createConsumerInterface<IceGrid::ObjectObserverPrx> ( context(), objectObj );

    nodeObserver_ = new NodeObserverI( context() );
    Ice::ObjectPtr nodeObj = nodeObserver_;
    IceGrid::NodeObserverPrx nodeObserverPrx =
        orcaice::createConsumerInterface<IceGrid::NodeObserverPrx> ( context(), nodeObj );

    session_->setObservers( registryObserverPrx, 
                            nodeObserverPrx,
                            applicationObserverPrx,
                            adapterObserverPrx,
                            objectObserverPrx );
}

void 
Component::stop()
{
    //
    // Destroy the sessionKeepAlive_ thread and the sesion object otherwise
    // the session_ will be kept allocated until the timeout occurs.
    // Destroying the session_ will release all allocated objects.
    //
    sessionKeepAlive_->destroy();
    sessionKeepAlive_->getThreadControl().join();
    session_->destroy();
}

} // namespace
