/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "velocitycontrol2dImpl.h"
#include "util.h"

using namespace std;

namespace orcaifaceimpl {

//////////////////////////////////////////////////////////////////////

//
// This is the implementation of the slice-defined interface
//
class VelocityControl2dI : public orca::VelocityControl2d
{
public:
    VelocityControl2dI( VelocityControl2dImpl &impl )
        : impl_(impl) {}

    virtual orca::VehicleDescription getDescription( const Ice::Current& c ) const
        { return impl_.internalGetDescription(); }

    virtual void setCommand(const ::orca::VelocityControl2dData& command, 
                            const ::Ice::Current& current )
        {  impl_.internalSetCommand( command ); }

private:
    VelocityControl2dImpl &impl_;
};

//////////////////////////////////////////////////////////////////////

VelocityControl2dImpl::VelocityControl2dImpl( 
            const orca::VehicleDescription& descr,
            const std::string &interfaceTag,
            const orcaice::Context &context )
    : description_(descr),
      interfaceName_(getInterfaceNameFromTag(context,interfaceTag)),
      topicName_(getTopicNameFromInterfaceName(context,interfaceName_)),
      context_(context)
{
}

VelocityControl2dImpl::VelocityControl2dImpl( 
            const orca::VehicleDescription& descr,
            const orcaice::Context &context,
            const std::string &interfaceName )
    : description_(descr),
      interfaceName_(interfaceName),
      topicName_(getTopicNameFromInterfaceName(context,interfaceName)),
      context_(context)
{
}

VelocityControl2dImpl::~VelocityControl2dImpl()
{
    tryRemoveInterface( context_, interfaceName_ );
}

void
VelocityControl2dImpl::initInterface()
{
    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new VelocityControl2dI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );
}

void 
VelocityControl2dImpl::initInterface( gbxiceutilacfr::Thread* thread, const std::string& subsysName, int retryInterval )
{
    ptr_ = new VelocityControl2dI( *this );
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_, thread, subsysName, retryInterval );
}

void
VelocityControl2dImpl::internalSetCommand(const ::orca::VelocityControl2dData& command )
{
    if ( this->hasNotifyHandler() ) {
        this->set( command );
    }
}

} // namespace
