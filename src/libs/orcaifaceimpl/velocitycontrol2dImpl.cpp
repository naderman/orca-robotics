/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
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
            const orcaice::Context &context ) :
    description_(descr),
    interfaceTag_(interfaceTag),
    context_(context)
{
}

VelocityControl2dImpl::~VelocityControl2dImpl()
{
    tryRemovePtr( context_, ptr_ );
}

void
VelocityControl2dImpl::initInterface()
{
    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new VelocityControl2dI( *this );
    orcaice::createInterfaceWithTag( context_, ptr_, interfaceTag_ );
}

void 
VelocityControl2dImpl::initInterface( hydroutil::Thread* thread, int retryInterval )
{
    ptr_ = new VelocityControl2dI( *this );
    orcaice::createInterfaceWithTag( context_, ptr_, interfaceTag_, thread, retryInterval );
}

void
VelocityControl2dImpl::internalSetCommand(const ::orca::VelocityControl2dData& command )
{
    if ( this->hasNotifyHandler() ) {
        this->set( command );
    }
}

} // namespace
