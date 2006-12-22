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

#include <IceGrid/Registry.h>
#include <IceGrid/Observer.h>
#include <orcaice/orcaice.h>
#include <orcalog/orcalog.h>

#include "component.h"
#include "observerI.h"

using namespace std;
using namespace orca;
using namespace orcaice;

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

    try
    {
        session_ = registry->createAdminSession( "horse", "shit" );
        cout << "created session"<<endl;
    }
    catch(const IceGrid::PermissionDeniedException& ex)
    {
        cout << "permission denied:\n" << ex.reason << endl;
    }

    keepAlive_ = new SessionKeepAliveThread( session_, registry->getSessionTimeout() / 2 );
    keepAlive_->start();

    regObserver_ = new RegistryObserverI( context() );
    nodeObserver_ = new NodeObserverI( context() );

    IceGrid::RegistryObserverPrx regObserverPrx =
        orcaice::createConsumerInterface<IceGrid::RegistryObserverPrx> ( context(), regObserver_ );
    IceGrid::NodeObserverPrx nodeObserverPrx =
        orcaice::createConsumerInterface<IceGrid::NodeObserverPrx> ( context(), nodeObserver_ );

    session_->setObservers( regObserverPrx, nodeObserverPrx );
    cout<<"set observers"<<endl;

    
}

void Component::stop()
{
    //
    // Destroy the keepAlive_ thread and the sesion object otherwise
    // the session_ will be kept allocated until the timeout occurs.
    // Destroying the session_ will release all allocated objects.
    //
    keepAlive_->destroy();
    keepAlive_->getThreadControl().join();
    session_->destroy();
}

} // namespace
