/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alexei Makarenko
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

CpuProbe::CpuProbe( const orca::FQInterfaceName & name, orcaprobe::DisplayDriver & display,
                                const orcaice::Context & context )
    : InterfaceProbe(name,display,context)
{
    id_ = "::orca::Cpu";
    
    addOperation( "getInfo" );
    addOperation( "getData" );
}

int 
CpuProbe::loadOperationEvent( const int index, orcacm::OperationData & data )
{
    switch ( index )
    {
    case orcaprobe::UserIndex :
        return loadGetInfo();
    case orcaprobe::UserIndex+1 :
        return loadGetData();
    }
    return 1;
}

int 
CpuProbe::loadGetInfo()
{
    orca::CpuInfoPtr data;
    
    try
    {
        orca::CpuPrx derivedPrx = orca::CpuPrx::checkedCast(prx_);
        data = derivedPrx->getInfo();
    }
    catch( const Ice::Exception & e )
    {
        return 1;
    }

    cout<<orcaice::toString(data)<<endl;
    return 0;
}

int 
CpuProbe::loadGetData()
{
    orca::CpuDataPtr data;
    
    try
    {
        orca::CpuPrx derivedPrx = orca::CpuPrx::checkedCast(prx_);
        data = derivedPrx->getData();
    }
    catch( const Ice::Exception & e )
    {
        return 1;
    }

    cout<<orcaice::toString(data)<<endl;
    return 0;
}
