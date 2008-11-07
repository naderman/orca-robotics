/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcacm/orcacm.h>
#include <orcaprobe/orcaprobe.h>
#include <orcaifacestring/velocitycontrol2d.h>

#include "velocitycontrol2dprobe.h"

using namespace std;
using namespace orcaprobefactory;

VelocityControl2dProbe::VelocityControl2dProbe( const orca::FQInterfaceName& name, const Ice::ObjectPrx& adminPrx, orcaprobe::AbstractDisplay& display,
                                const orcaice::Context& context )
    : InterfaceProbe(name,adminPrx,display,context)
{
    id_ = "::orca::VelocityControl2d";
    
    addOperation( "getDescription" );
}
    
int 
VelocityControl2dProbe::loadOperationEvent( const int index, orcacm::OperationData& data )
{
    switch ( index )
    {
    case orcaprobe::UserIndex :
        return loadGetDescription( data );
    }
    return 1;
}

int 
VelocityControl2dProbe::loadGetDescription( orcacm::OperationData& data )
{
    orca::VehicleDescription result;
    try
    {
        orca::VelocityControl2dPrx derivedPrx = orca::VelocityControl2dPrx::checkedCast(prx_);
        result = derivedPrx->getDescription();
        orcaprobe::reportResult( data, "data", ifacestring::toString(result) );
    }
    catch( const Ice::Exception& e )
    {
        stringstream ss;
        ss<<e<<endl;
        orcaprobe::reportException( data, ss.str() );
    }
    return 0;
}
