/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_COMPONENT_CONTEXT_H
#define ORCAICE_COMPONENT_CONTEXT_H

#include <Ice/Ice.h>

#include <orca/ocm.h>
#include <orcaice/home.h>
#include <hydroutil/tracer.h>
#include <hydroutil/status.h>
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

    //! Conveniance function which returns smart pointer to the component's properties.
    //! Same as calling communicator()->getProperties()
    Ice::PropertiesPtr     properties() const { return communicator_->getProperties(); };

    //! Access to setting home information.
    orcaice::Home&         home() const { return *home_; };
    //! Access tracing functions.
    hydroutil::Tracer&     tracer() const { return *tracer_; };
    //! Access status functions.
    hydroutil::Status&     status() const { return *status_; };


    //! Calls Component::activate(). This function is useful when component activation
    //! (technically Ice::ObjectAdapter activation) must be delayed until after something is 
    //! initialized in the child thread.
    void activate();

    // WARNING to internal developers:
    // Do not call this function before the context is fully initialized, i.e.
    // before Component::start() is called.
    //! Repackages itself into the form useful for initializing Hydro drivers.
    //! Strips 'tag()+".Config."' from the front of all properties.
    hydroutil::Context toHydroContext() const;

    // WARNING to internal developers:
    // Do not call this function before the context is fully initialized, i.e.
    // before Component::start() is called.
    //! Repackages itself into the form useful for initializing Hydro drivers.
    //! Strips 'prefixToStrip' from the front of all properties.
    //! Does not copy properties which do not begin with prefixToStrip.
    hydroutil::Context toHydroContext( const std::string &prefixToStrip ) const;

private:

    // this function should only be called by Component (it can because it's a friend)
    void init( const orca::FQComponentName& name,
               const bool isApp,
               const Ice::ObjectAdapterPtr& adapter,
               orcaice::Component* comp  );

    std::string            tag_;
    orca::FQComponentName  name_;
    bool                   isApplication_;
    Ice::CommunicatorPtr   communicator_;
    Ice::ObjectAdapterPtr  adapter_;

    // need this pointer to call activate()
    Component*             component_; 

    // these could be auto_ptr's
    orcaice::Home*         home_;
    hydroutil::Tracer*     tracer_;
    hydroutil::Status*     status_;
};

} // end namespace

#endif
