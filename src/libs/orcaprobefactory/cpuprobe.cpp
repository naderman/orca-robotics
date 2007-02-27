/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcacm/orcacm.h>

#include "cpuprobe.h"
#include <orcaprobe/orcaprobe.h>

using namespace std;
using namespace orcaprobefactory;

CpuProbe::CpuProbe( const orca::FQInterfaceName& name, orcaprobe::DisplayDriver& display,
                                const orcaice::Context& context )
    : InterfaceProbe(name,display,context)
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
    orca::CpuInfo result;
    try
    {
        orca::CpuPrx derivedPrx = orca::CpuPrx::checkedCast(prx_);
        result = derivedPrx->getInfo();
        orcaprobe::reportResult( data, "data", orcaice::toString(result) );
    }
    catch( const Ice::Exception& e )
    {
        stringstream ss;
        ss<<e<<endl;
        orcaprobe::reportException( data, ss.str() );
    }
    return 0;
}

int 
CpuProbe::loadGetData( orcacm::OperationData& data )
{
    orca::CpuData result;
    try
    {
        orca::CpuPrx derivedPrx = orca::CpuPrx::checkedCast(prx_);
        result = derivedPrx->getData();
        orcaprobe::reportResult( data, "data", orcaice::toString(result) );
    }
    catch( const Ice::Exception& e )
    {
        stringstream ss;
        ss<<e<<endl;
        orcaprobe::reportException( data, ss.str() );
    }
    return 0;
}
