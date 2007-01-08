/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <orcaice/orcaice.h>

#include "velocitycontrol2dI.h"

using namespace std;
using namespace robot2d;

VelocityControl2dI::VelocityControl2dI( const orca::VelocityControl2dDescription& descr, 
                   orcaice::Notify<orca::VelocityControl2dData>& commandPipe )
    : descr_(descr),
      commandPipe_(commandPipe)
{
}

orca::VelocityControl2dDescription
VelocityControl2dI::getDescription(const Ice::Current& current) const
{
    return descr_;
}

// Store incoming command in a proxy, it will be handled by the driver at the next opportunity.
void
VelocityControl2dI::setCommand(const ::orca::VelocityControl2dData& command, const ::Ice::Current& )
{
    // this is executed directly to hardware and may throw an orca::HardwareException
    commandPipe_.set( command );
}
