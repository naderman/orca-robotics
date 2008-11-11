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
#include <orcaifaceutil/cpu.h>

#include "cpuprobe.h"
#include <orcaprobe/orcaprobe.h>

using namespace std;
using namespace orcaprobefactory;

CpuProbe::CpuProbe( const orca::FQInterfaceName& name, const Ice::ObjectPrx& adminPrx, orcaprobe::AbstractDisplay& display,
                                const orcaice::Context& context )
    : InterfaceProbe(name,adminPrx,display,context)
{
    id_ = "::orca::Cpu";
    
    addOperation( "getInfo" );
    addOperation( "getData" );
}

int 
CpuProbe::loadOperationEvent( const int index, orcacm::OperationData& data )
{
    switch ( index )
    {
    case orcaprobe::UserIndex :
        return loadGetInfo( data );
    case orcaprobe::UserIndex+1 :
        return loadGetData( data );
    }
    return 1;
}

int 
CpuProbe::loadGetInfo( orcacm::OperationData& data )
{
    orca::CpuPrx derivedPrx = orca::CpuPrx::checkedCast(prx_);
    orcaprobe::reportResult( data, "data", ifaceutil::toString( derivedPrx->getInfo() ) );
    return 0;
}

int 
CpuProbe::loadGetData( orcacm::OperationData& data )
{
    orca::CpuPrx derivedPrx = orca::CpuPrx::checkedCast(prx_);
    orcaprobe::reportResult( data, "data", ifaceutil::toString( derivedPrx->getData() ) );
    return 0;
}
