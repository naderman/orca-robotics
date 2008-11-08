/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_COMPONENT_H
#define ORCAICE_COMPONENT_H

#include <orcaice/context.h>
#include <hydroutil/uncopyable.h>
#include <gbxsickacfr/gbxiceutilacfr/thread.h>

#include <orcaice/homeImpl.h>
#include <orcaice/statusImpl.h>
#include <orcaice/tracerImpl.h>

namespace orcaice
{

class Status;

/*! 
This enum type is used to describe which standard interfaces the component
will provide.

Flags can be combined. For example to initialize Home and Status but not Tracer.
@verbatim
orcaice::ComponentInterfaceFlag flag = orcaice::HomeInterface | orcaice::StatusInterface;
@endverbatim
*/
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
//!   Component initialisation code can be put in two places:
//!
//!   1. Component Constructor
//!
//!      - In the constructor, none of the orcaice machinery is initialised.  This
//!        means that no remote calls can be made (the communicator isn't initialised yet),
//!        and the tracer isn't available yet.
//!      - Any code that should be executed before any remote calls are made belongs in 
//!        the Component constructor.
//!
//!   2. Component::start()
//!
//!      - start() is called after all resources in Context are initialised.
//!        - Home and Status interfaces are initialized from a separate thread and
//!          it may take multiple attempts to do this.
//!      - This is the place to launch threads from and get out.
//!        - Don't do anything that might loop forever in here, otherwise it won't be possible
//!          to bring the component down cleanly.
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
//! @author Alex Brooks, Alexei Makarenko
//!
//! @sa Application, Service, Context
//! @sa Tracer, Status
//!
class Component : public hydroutil::Uncopyable
{
// these are declared friends so they can call init(), tag(), finalise()
friend class Application;
friend class Service;
// this one needs to call activate().
friend class Context;

public:
/*! 
Takes the text tag with which to identify it in the config files. The @e flag
specifies what standard interfaces to initialize. It is also possible to configure
standard interfaces with the following configuration parameters.
@verbatim
Orca.Component.EnableTracer
Orca.Component.EnableStatus
Orca.Component.EnableHome
@endverbatim

Inside this contructor the component context is not initialized yet and cannot be used.
*/
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
    virtual const std::string help( const std::string& executable ) const;

    /*! This function is called by Application on startup (including when the executable is 
    //! called with @c --version command line option). Standard Orca components return an
    //! empty string. Component from external project may reimplement this function to supply
    //! the project version, which is probably different from the Orca version. 
    //! Example:
@verbatim
virtual const std::string version() const { return std::string(PROJECT_VERSION); };
@endverbatim
    //! Note that PROJECT_VERSION is defined automatically for all projects.
    */
    virtual const std::string version() const { return std::string(""); };

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
    //!context().tracer().info("Everything is OK");
    //!MainLoop myloop( context() );
    //! @endverbatim
    const Context& context() const { return context_; };

private:

    // One of the container classes (Application or Service) will
    // call this function before calling start().
    // This is the reason for them to be friends.
    void init( const orca::FQComponentName& name,
               const bool isApp,
               const Ice::ObjectAdapterPtr& adapter );

    // Cleans up internal resources after stop() was called
    // but before complete shutdown.
    void finalise();

    // Only Service should need to use this when the IceBox tells it
    // what the actual tag is.
    void setTag( const std::string& t ) { context_.tag_ = t; };

    // initialize component services
    hydroutil::History* initHistory();
    void getNetworkProperties();

    // Save init flags (for the period between the constructor and init())
    ComponentInterfaceFlag interfaceFlag_;

    HomeImplPtr home_;
    StatusImplPtr status_;
    TracerImplPtr tracer_;

    // This thread allows us to do house-keeping stuff and manage Status.
    gbxiceutilacfr::ThreadPtr componentThread_;

    // Component's context
    Context context_;
};

} // end namespace

#endif
