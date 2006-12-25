/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_COMPONENT_H
#define ORCAICE_COMPONENT_H

#include <orcaice/context.h>

namespace orcaice
{

//! This enum type is used to describe which standard interfaces the component
//! will provide.
enum ComponentInterfaceFlag {
    //! No standard interfaces
    NoStandardInterfaces    = 0x000,
    //! Home interface only.
    HomeInterface           = 0x001,
    //! Status interface only.
    StatusInterface         = 0x010,
    //! Tracer interface only.
    TracerInterface         = 0x100,
    //! All standard interfaces.
    AllStandardInterfaces   = HomeInterface | StatusInterface | TracerInterface
};

//!
//! @brief Base class for all Components using libOrcaIce
//!
//! Deriving from this makes it easy to use code as either a
//! stand-alone application (using orcaice::Application)
//! or as a service in an IceBox (using orcaice::Service). Two functions
//! must be implemented: start and stop.
//!
//! Most of the state of the component is summarized in its
//! Component::context. The information is read-only because it is set 
//! prior to the component's initialization.
//!
//! @par Component Initialisation
//! 
//!   Component initialisation code can be put in three places:
//!
//!   1. Component Constructor
//!
//!      - In the constructor, none of the orcaice machinery is initialised.  This
//!        means that no remote calls can be made (the communicator isn't initialised yet),
//!        and the tracer isn't available yet.
//!      - Any code that should be executed before any remote calls are made belongs in the Component constructor.
//!
//!   2. Component::init()
//!
//!      - This is called after initialisation of the context and Home interface.
//!      - This code is executed in the Application or Service's thread.
//!
//!   3. Component::start()
//!
//!      - This is the place to launch threads from.
//!      - start() is called immediately after init(), so the same things (eg context) are available.
//!
//! @par Component Destruction
//!
//!   Clean-up code belongs in one of two places:
//!
//!   1. Component::stop()
//!
//!     - The context and communicator are still available at this point
//!
//!   2. Component Destructor
//!
//!     - The context and communicator are no longer available
//!
//! @author Alex Brooks
//!
//! @sa Application, Service, Context
//!
class Component
{
// these are declared friends so they can call init() and tag()
friend class Application;
friend class Service;
// this one needs to call activate().
friend class Context;

public:
    //! Takes the text tag with which to identify it in the config files. The @e flag
    //! specifies what standard interfaces to initialize.
    Component( const std::string& tag, ComponentInterfaceFlag flag=AllStandardInterfaces );
    virtual ~Component() {};

    //! This function is called by the component's container (Application or Service).
    //! It should return immediately, possibly after launching a thread. GUI components
    //! are an exception to this rule, they may run in the calling thread provided that
    //! the Ctrl-C handler was not installed (see Application, Service).
    virtual void start()=0;

    //! This function is called by the component's container (Application or Service)
    //! when the component is ordered to stop execution.
    virtual void stop()=0;

    //! This function is called by Application when the executable is called with @c --help
    //! command line option.
    virtual std::string help();

protected:

    //! Activates the component's adapter.
    //! Activation makes provided interfaces accessible from the outside world.
    //! It also tries to register the adapter with the IceGrid Registry. 
    //! If the registry is unreachable, the adapter is not fully activated.
    //!
    //! An Orca configuration property Orca.RequireRegistry determines what happens if the Registry
    //! is not available. If RequireRegistry=1 (default) and the registry is unreachable,
    //! an orcaice::NetworkException is thrown. In this case it is safe to call activate again later, 
    //! hoping that the regisry will become reachable. If RequireRegistry=0 no exception is thrown.
    void activate();

    //! Component's "context", which contains component's naming and networking information.
    //! It can be used directly or passed to threads and classes. For example:
    //! @verbatim
    //!context().tracer()->info("Everything is OK");
    //!MainLoop myloop( context() );
    //! @endverbatim
    const Context& context() const { return context_; };

    //! Convenience shortcut to component's tag. Often used when parsing configuration files.
    //! Same result as context().tag().
    const std::string& tag() const { return context_.tag_; };
    //! Convenience shortcut to component's local tracer API.
    //! Same result as context().tracer().
    Tracer* tracer() const { return context_.tracer_; };
    //! Convenience shortcut to component's local status API.
    //! Same result as context().status().
    Status* status() const { return context_.status_; };
    //! Convenience shortcut to component's local home API.
    //! Same result as context().home().
    Home* home() const { return context_.home_; };
    //! Convenience shortcut to component's configuration properties.
    //! Same result as context().properties().
    Ice::PropertiesPtr properties() const { return context_.properties(); };

    //! Describe which standard interfaces this component will provide.
    ComponentInterfaceFlag interfaceFlag() const { return interfaceFlag_; };

private:

    // Not implemented; prevents accidental use.
    Component( const Component& );
    Component& operator= ( const Component& );

    // One of the container classes (Application or Service) will
    // call this function before calling start().
    // This is the reason for them to be friends.
    void init( const orca::FQComponentName& name,
                const bool isApp,
                const Ice::ObjectAdapterPtr& adapter );

    // Only Service should need to use this when the IceBox tells it
    // what the actual tag is.
    void setTag( const std::string& t ) { context_.tag_ = t; };

    // Component's context
    Context context_;

    // Save init flags
    ComponentInterfaceFlag interfaceFlag_;

    // initialize component services
    orcaice::Home*   initHome();
    orcaice::Tracer* initTracer();
    orcaice::Status* initStatus();

    // keep the smart pointer so it's not destroyed with the adapter
    // (i think we only need to do it with tracer)
    Ice::ObjectPtr tracerObj_;
    Ice::ObjectPtr statusObj_;
};

} // end namespace

#endif
