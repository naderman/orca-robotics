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

#include "velocitycontrol2diface.h"
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
    VelocityControl2dI( VelocityControl2dIface& iface )
        : iface_(iface) {}

    virtual orca::VehicleDescription getDescription( const Ice::Current& c ) const
        { return iface_.getDescription(); }

    virtual void setCommand(const ::orca::VelocityControl2dData& command, 
                            const ::Ice::Current& current )
        {  iface_.setCommand( command ); }

private:
    VelocityControl2dIface &iface_;
};

//////////////////////////////////////////////////////////////////////

VelocityControl2dIface::VelocityControl2dIface( 
            const orca::VehicleDescription& descr,
            const std::string &ifaceTag,
            const orcaice::Context &context ) :
    description_(descr),
    ifaceTag_(ifaceTag),
    context_(context)
{
}

VelocityControl2dIface::~VelocityControl2dIface()
{
    tryRemovePtr( context_, ptr_ );
}

void
VelocityControl2dIface::initInterface()
{
    // Register with the adapter
    // We don't have to clean up the memory we're allocating here, because
    // we're holding it in a smart pointer which will clean up when it's done.
    ptr_ = new VelocityControl2dI( *this );
    orcaice::createInterfaceWithTag( context_, ptr_, ifaceTag_ );
}

void
VelocityControl2dIface::setCommand(const ::orca::VelocityControl2dData& command )
{
    if ( this->hasNotifyHandler() ) {
        this->set( command );
    }
}

} // namespace
