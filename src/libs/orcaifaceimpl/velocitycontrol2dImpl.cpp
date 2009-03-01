/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include "velocitycontrol2dImpl.h"

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
    virtual orca::VehicleDescription getDescription( const Ice::Current& )
        { return impl_.internalGetDescription(); }
    virtual void setCommand(const ::orca::VelocityControl2dData& command, const ::Ice::Current& current )
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
      interfaceName_(orcaice::getProvidedInterface(context,interfaceTag).iface),
      context_(context)
{
    init();
}

VelocityControl2dImpl::VelocityControl2dImpl( 
            const orca::VehicleDescription& descr,
            const orcaice::Context &context,
            const std::string &interfaceName )
    : description_(descr),
      interfaceName_(interfaceName),
      context_(context)
{
    init();
}

VelocityControl2dImpl::~VelocityControl2dImpl()
{
    orcaice::tryRemoveInterface( context_, interfaceName_ );
}

void
VelocityControl2dImpl::init()
{
    ptr_ = new VelocityControl2dI( *this );
}

void
VelocityControl2dImpl::initInterface()
{
    orcaice::createInterfaceWithString( context_, ptr_, interfaceName_ );
}

void 
VelocityControl2dImpl::initInterface( gbxiceutilacfr::Thread* thread, const std::string& subsysName, int retryInterval )
{
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
