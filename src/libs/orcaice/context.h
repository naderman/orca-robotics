/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_COMPONENT_CONTEXT_H
#define ORCAICE_COMPONENT_CONTEXT_H

#include <Ice/Ice.h>

#include <orca/orca.h>
#include <orca/status.h>
#include <orcaice/tracer.h>

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
 *  smart pointer types are themselves thread-safe. This means that once a copy of
 *  of Context is created, it is safe to use it from different threads.
 */
class Context
{
friend class Component;

public:
    Context();

    //! Component's tag used in configuration files.
    std::string            tag() const { return tag_; };
    //! Component's fully-qualified name given to the Registry.
    orca::FQComponentName  name() const { return name_; };
    //! @brief Is the component executed inside and application or a service?
    //! Returns TRUE if this instance of the Component is used in a stand-alone application.
    //! Otherwise, returns FALSE, meaning that it's used in a IceBox service.
    bool                   isApplication() const { return isApplication_; };
    //! Returns smart pointer to the component's communicator.
    Ice::CommunicatorPtr   communicator() const { return communicator_; };
    //! Returns smart pointer to the component's adapter.
    Ice::ObjectAdapterPtr  adapter() const { return adapter_; };
    //! Returns smart pointer to the component's properties.
    Ice::PropertiesPtr     properties() const { return communicator_->getProperties(); };
    //! Access logging functions through standard Ice::Logger interface.
    //! Use @ref tracer instead.
    Ice::LoggerPtr         logger() const { return communicator_->getLogger(); };
    //! Access tracing functions.
    orcaice::Tracer*       tracer() const { return tracer_; };

    //! Convenience function which calls Component::activate.
    void activate();

    // Component itself
    // This function may be removed in the future.
    // orcaice::Component*    component() const;

private:

    // this function should only be called by Component (it can because it's a friend)
    void init( const orca::FQComponentName& name,
               const bool isApp,
               const Ice::ObjectAdapterPtr & adapter,
               orcaice::Component* comp  );

    std::string            tag_;
    orca::FQComponentName  name_;
    bool                   isApplication_;
    Ice::CommunicatorPtr   communicator_;
    Ice::ObjectAdapterPtr  adapter_;

    // need this pointer to call activate()
    Component*             component_; 
    orcaice::Tracer*       tracer_;
};

} // end namespace

#endif
