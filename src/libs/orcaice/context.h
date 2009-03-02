/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_COMPONENT_CONTEXT_H
#define ORCAICE_COMPONENT_CONTEXT_H

// We carefully select the minimum subset of Ice include file we commonly need.
// It would be easier to just include <Ice/Ice.h> at the expense of more includes
// and slower compilation. We are trying to be careful because this file is included
// by practically everything in Orca.
#include <Ice/Communicator.h>
#include <Ice/ObjectAdapter.h>
#include <Ice/LocalException.h>

#include <orca/ocm.h>
#include <orcaice/home.h>
#include <gbxutilacfr/tracer.h>
#include <gbxutilacfr/status.h>
#include <hydroutil/history.h>
#include <hydroutil/context.h>

namespace orcaice
{

class Component;

/*!
 *  @brief Component's naming and networking information.
 *
 *  Contains pointers to component's registered name plus its communicator,
 *  adapter, etc. It makes it easy to pass all this information to the network and
 *  hardware handlers from the class derived from Component.
 *
 *  A note on thread safety. None of access functions are thread-safe. However, all
 *  object which are referenced (with pointers and smart pointers) are themselves thread-safe. 
 *  
 *  This means that once a copy of of Context is created, it is safe to use it from different threads.
 *  Pass context by const reference and store a copy, e.g. a class definition would look like this:
@verbatim
class MyClass
{
public:
    MyClass( const orcaice::Context & context )
        : context_(context) {};
private:
    orcaice::Context context_;
}
@endverbatim
 */
class Context
{
friend class Component;

public:
    // We need the empty constructor because the original one is constucted a bit at a time
    // during component initialization.
    // We don't document this constuctor because the user typically only sees the copy constructor.
    Context();

    //! Component's tag used in configuration files.
    const std::string&     tag() const { return tag_; };

    //! Component's fully-qualified name given to the Registry.
    const orca::FQComponentName& name() const { return name_; };

    //! @brief Is this component executed inside and application or a service?
    //! Returns TRUE if this instance of the Component is used in a stand-alone application.
    //! Otherwise, returns FALSE, meaning that it's used inside an IceBox service.
    bool                   isApplication() const { return isApplication_; };

    //! Returns smart pointer to the component's communicator.
    Ice::CommunicatorPtr   communicator() const { return communicator_; };

    //! Returns smart pointer to the component's adapter.
    Ice::ObjectAdapterPtr  adapter() const { return adapter_; };

    //! Convenience function which returns smart pointer to the component's properties.
    //! Same as calling communicator()->getProperties()
    Ice::PropertiesPtr     properties() const { return communicator_->getProperties(); };

    //! Access to setting home information.
    orcaice::Home&         home() const { return *home_; };
    //! Access tracing functions.
    gbxutilacfr::Tracer&     tracer() const { return *tracer_; };
    //! Access status functions.
    gbxutilacfr::Status&     status() const { return *status_; };
    //! Access history functions.
    hydroutil::History&    history() const { return *history_; };


    //! Actiates server functionality of the component. This function is useful when component activation
    //! (technically Ice::ObjectAdapter activation) must be delayed until after something is 
    //! initialized in the child thread.
    void activate();

    //! Returns TRUE if the component is in the process of deactivating itself (the communicator
    //! is already destroyed).
    bool isDeactivating();

    //! Triggers component shutdown.
    //!
    //! Do not call this from inside functions implementing Ice remote calls. (alexm: this restriction
    //! may be outdated since we now call Communicator::shutdown() instead of destroy().)
    void shutdown() const;

    // WARNING to internal developers:
    // Do not call these functions before the context is fully initialized, i.e.
    // before Component::start() is called.
    //! Repackages itself into the form useful for initializing Hydro drivers.
    //! Strips 'prefixToStrip' from the front of all properties.
    //! Does not copy properties which do not begin with prefixToStrip.
    //! Transfers the following config parameters from Orca configs to Hydro configs:
    //! - Orca.Warn.DefaultProperty
    hydroutil::Context toHydroContext( const std::string &prefixToStrip ) const;

    //! Conveniance function which is a shortcut for the function above with
    //! @c prefixToStrip = tag()+".Config."
    hydroutil::Context toHydroContext() const;

    //! Returns debugging string describing all contents.
    std::string toString() const;

private:

    // this function should only be called by Component (it can because it's a friend)
    void init( const orca::FQComponentName& name,
               const bool isApp,
               const Ice::ObjectAdapterPtr& adapter );

    std::string            tag_;
    orca::FQComponentName  name_;
    bool                   isApplication_;
    Ice::CommunicatorPtr   communicator_;
    Ice::ObjectAdapterPtr  adapter_;

    // Context does not take responsibility for these objects, i.e. does not destroy them
    orcaice::Home*         home_;
    gbxutilacfr::Tracer*   tracer_;
    gbxutilacfr::Status*   status_;
    hydroutil::History*    history_;
};

} // end namespace

#endif
