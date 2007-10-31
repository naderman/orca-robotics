/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCAIFACEIMPL_VELOCITYCONTROL2D_IMPL_H
#define ORCA2_ORCAIFACEIMPL_VELOCITYCONTROL2D_IMPL_H

#include <orca/velocitycontrol2d.h>
#include <IceStorm/IceStorm.h>

#include <hydroutil/proxy.h>
#include <hydroutil/notify.h>
#include <orcaice/context.h>

namespace hydroutil {
    class Thread;
}

namespace orcaifaceimpl {

//!
//! Implements the VelocityControl2d interface: Handles remote calls.
//!
class VelocityControl2dImpl : public IceUtil::Shared,
                               public hydroutil::Notify<orca::VelocityControl2dData>
{
friend class VelocityControl2dI;

public:
    //! Constructor using interfaceTag (may throw ConfigFileException)
    VelocityControl2dImpl( const orca::VehicleDescription& descr,
                           const std::string& interfaceTag,
                           const orcaice::Context& context );
    //! constructor using interfaceName
    VelocityControl2dImpl( const orca::VehicleDescription& descr,
                           const orcaice::Context& context,
                           const std::string& interfaceName );
    ~VelocityControl2dImpl();

    //! Sets up interface and connects to IceStorm. May throw hydroutil::Exceptions.
    void initInterface();

    //! Sets up interface and connects to IceStorm. Catches all exceptions and retries
    //! until sucessful. At every iteration, checks if the thread was stopped.
    void initInterface( hydroutil::Thread* thread, int retryInterval=2 );

private:

    // remote call implementations, mimic (but do not inherit) the orca interface
    orca::VehicleDescription internalGetDescription() const
        { return description_; }
    void internalSetCommand( const ::orca::VelocityControl2dData& );

    const orca::VehicleDescription    description_;
    const std::string                 interfaceName_;
    const std::string                 topicName_;
    IceStorm::TopicPrx                topicPrx_;

    orcaice::Context                  context_;

    // Hang onto this so we can remove from the adapter and control when things get deleted
    Ice::ObjectPtr          ptr_;
};
typedef IceUtil::Handle<VelocityControl2dImpl> VelocityControl2dImplPtr;

} // namespace

#endif
