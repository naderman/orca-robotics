#ifndef COMPONENTTHREAD_H
#define COMPONENTTHREAD_H

#include <orcaice/thread.h>
#include <orcaice/context.h>
#include <orcaice/status.h>
#include <orcaice/component.h>

namespace orcaice {

//
// This thread is started by the component, and used to:
// - repeatedly try to register the Home interface, if the first one failed.
// - check on component Status.
//
// @author Alex Brooks
//
class ComponentThread : public orcaice::Thread
{

public: 

    ComponentThread( const Ice::ObjectPrx   &homePrx,
                     Status                 &status,
                     ComponentInterfaceFlag  interfaceFlag,
                     const orcaice::Context &context );
    ~ComponentThread();

    virtual void run();

private: 

    // Returns: true = success
    bool tryRegisterHome();

    bool registeredHome_;

    Ice::ObjectPrx          homePrx_;
    Status                 &status_;
    ComponentInterfaceFlag  interfaceFlag_;
    orcaice::Context        context_;
};

}

#endif
