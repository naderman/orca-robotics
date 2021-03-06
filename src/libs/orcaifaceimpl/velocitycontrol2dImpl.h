/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCAIFACEIMPL_VELOCITYCONTROL2D_IMPL_H
#define ORCA_ORCAIFACEIMPL_VELOCITYCONTROL2D_IMPL_H

#include <orca/velocitycontrol2d.h>
#include <gbxsickacfr/gbxiceutilacfr/store.h>
#include <orcaice/context.h>

namespace gbxutilacfr { class Stoppable; }

namespace orcaifaceimpl {

class AbstractVelocityControl2dCallback {
 public:
    virtual ~AbstractVelocityControl2dCallback() {}
    virtual void setCommand( const orca::VelocityControl2dData &command )=0;
};

//!
//! Implements the VelocityControl2d interface: Handles remote calls.
//!
class VelocityControl2dImpl : public IceUtil::Shared
{
friend class VelocityControl2dI;

public:
    //! Constructor using interfaceTag (may throw ConfigFileException)
    VelocityControl2dImpl( AbstractVelocityControl2dCallback &callback,
                           const orca::VehicleDescription& descr,
                           const std::string& interfaceTag,
                           const orcaice::Context& context );
    //! constructor using interfaceName
    VelocityControl2dImpl( AbstractVelocityControl2dCallback &callback,
                           const orca::VehicleDescription& descr,
                           const orcaice::Context& context,
                           const std::string& interfaceName );
    ~VelocityControl2dImpl();

    //! Sets up interface and connects to IceStorm. May throw gbxutilacfr::Exceptions.
    void initInterface();

    //! Sets up interface and connects to IceStorm. Catches all exceptions and retries
    //! until sucessful. At every iteration, checks if the activity (e.g. thread) was stopped.
    void initInterface( gbxutilacfr::Stoppable* activity, const std::string& subsysName="", int retryInterval=2 );

private:
    void init();

    // remote call implementations, mimic (but do not inherit) the orca interface
    orca::VehicleDescription internalGetDescription() const
        { return description_; }
    void internalSetCommand( const ::orca::VelocityControl2dData& );

    AbstractVelocityControl2dCallback &callback_;
    const orca::VehicleDescription     description_;

    Ice::ObjectPtr ptr_;
    const std::string interfaceName_;
    orcaice::Context context_;
};
typedef IceUtil::Handle<VelocityControl2dImpl> VelocityControl2dImplPtr;

} // namespace

#endif
