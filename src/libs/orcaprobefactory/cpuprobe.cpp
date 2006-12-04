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
#include <iomanip>    // for setw()
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
    
    operations_.push_back( "getInfo" );
    operations_.push_back( "getData" );
}

int 
CpuProbe::loadOperation( const int index, orcacm::OperationData & data )
{
    //cout<<"loading home operation "<<index<<endl;

    int ret = 1;
    
    switch ( index )
    {
    case 0 :
        ret = loadGetInfo();
        break;
    case 1 :
        ret = loadGetData();
        break;
    default :
        ret = 1;
        cout<<"unknown operation index"<<endl;
        break;
    }

    return ret;
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
