#include "sessioncreationcallback.h"
#include <iostream>
#include <orcaice/orcaice.h>

using namespace std;

namespace icegridmon {

SessionCreationCallback::SessionCreationCallback( const orcaice::Context &context )
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

bool 
SessionCreationCallback::actionOnSessionCreation( IceGrid::AdminSessionPrx session )
{
    session->setObservers( registryObserverPrx_, 
                           nodeObserverPrx_,
                           applicationObserverPrx_,
                           adapterObserverPrx_,
                           objectObserverPrx_ );
    return true;
}

}
