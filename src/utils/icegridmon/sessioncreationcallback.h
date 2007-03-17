#ifndef SESSIONCREATIONCALLBACK_H
#define SESSIONCREATIONCALLBACK_H

#include <orcaicegrid/sessionmanager.h>
#include <IceGrid/Registry.h>
#include <orcaice/context.h>
#include "registryobserverI.h"
#include "applicationobserverI.h"
#include "adapterobserverI.h"
#include "objectobserverI.h"
#include "nodeobserverI.h"

namespace icegridmon {

//!
//! @author Alex Brooks
//!
class SessionCreationCallback : public orcaicegrid::SessionCreationCallback
{

public: 

    SessionCreationCallback( const orcaice::Context &context );

    bool actionOnSessionCreation( IceGrid::AdminSessionPrx session );

private: 

    // Observers
    RegistryObserverI               *registryObserver_;
    IceGrid::RegistryObserverPrx     registryObserverPrx_;
    ApplicationObserverI            *applicationObserver_;
    IceGrid::ApplicationObserverPrx  applicationObserverPrx_;
    AdapterObserverI                *adapterObserver_;
    IceGrid::AdapterObserverPrx      adapterObserverPrx_;
    ObjectObserverI                 *objectObserver_;
    IceGrid::ObjectObserverPrx       objectObserverPrx_;
    NodeObserverI                   *nodeObserver_;    
    IceGrid::NodeObserverPrx         nodeObserverPrx_;

    orcaice::Context context_;
};

}

#endif
