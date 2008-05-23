/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <iomanip>    // for setw()
#include <orcaice/orcaice.h>
#include <orcacm/orcacm.h>
#include <orcaifacestring/home.h>

#include "homeprobe.h"
#include <orcaprobe/orcaprobe.h>

using namespace std;
using namespace orcaprobefactory;

HomeProbe::HomeProbe( const orca::FQInterfaceName& name, orcaprobe::AbstractDisplay& display,
                                const orcaice::Context& context )
    : InterfaceProbe(name,display,context)
{
    id_ = "::orca::Home";
    
    addOperation( "getInterfaces", "nonmutating HomeData getInterfaces()" );
    addOperation( "getProperties", "nonmutating ComponentProperties getProperties()" );
}

int 
HomeProbe::loadOperationEvent( const int index, orcacm::OperationData& data )
{    
    switch ( index )
    {
    case orcaprobe::UserIndex :
        return loadGetInterfaces( data );
    case orcaprobe::UserIndex+1 :
        return loadGetProperties( data );
    }
    return 1;
}

int 
HomeProbe::loadGetInterfaces( orcacm::OperationData& data )
{
    orca::HomePrx derivedPrx = orca::HomePrx::checkedCast(prx_);
    orcaprobe::reportResult( data, "data", ifacestring::toString( derivedPrx->getInterfaces() ) );
    return 0;
}

int 
HomeProbe::loadGetProperties( orcacm::OperationData& data )
{
    orca::HomePrx derivedPrx = orca::HomePrx::checkedCast(prx_);
    orcaprobe::reportResult( data, "data", ifacestring::toString( derivedPrx->getProperties() ) );
    return 0;
}
