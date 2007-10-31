/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_ICEBOX_SERVICE_H
#define ORCAICE_ICEBOX_SERVICE_H

#include <IceBox/IceBox.h>

namespace orcaice {

class Component;

//!
//! @brief Base class to use when putting Orca components into an IceBox
//!
//! This is a thin wrapper for the IceBox::Service class.
//! It specializes it by adding a few small Orca conventions,
//! such as starting a single adapter for the component.
//!
//! It runs the single orcaice::Component. The pointer to it must be set from the derived
//! class before @p start is called. The destructor deletes the component, so the derived classes
//! don't have to.
//!
//! @author Alex Brooks
//!
//! @sa Application, Component
//!
class Service : public IceBox::Service
{

public:

    /*!
     *   Unlike in Application, the communicator is created before the service is started.
     */
    Service();
    virtual ~Service();

    //! Implements virtual function called by IceBox on start-up.
    //! Input parameters @p name and @p args come from the service configuration line
    //! of the IceBox config file (the line which specifies @e create entry point ).
    virtual void start(const ::std::string        & name,
                       const Ice::CommunicatorPtr & communicator,
                       const Ice::StringSeq       & args);

    //! Implements virtual function called by IceBox on shut-down.
    virtual void stop();

protected:

    // A service contains only one Component. Derived services will need to set this
    // pointer so we make it protected.
    orcaice::Component *component_;
    
private:

    // By convention there is exactly one adapter per component and, therefore, per service
    // Keep the pointer to it here, so it does not get destroyed too soon.
    Ice::ObjectAdapterPtr   adapter_;

    // This is flag which is set to true by the first service which uses the shared
    // communicator (after it configures it with factory defaults and global settings)
    static bool _isSharedCommunicatorConfigured;
};

} // namespace

#endif
