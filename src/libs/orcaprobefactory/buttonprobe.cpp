/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcaprobe/orcaprobe.h>
#include <orcaifaceutil/button.h>

#include "buttonprobe.h"

using namespace std;
using namespace orcaprobefactory;

ButtonProbe::ButtonProbe( const orca::FQInterfaceName& name, const Ice::ObjectPrx& adminPrx, orcaprobe::AbstractDisplay& display,
                                const orcaice::Context& context )
   : InterfaceProbe(name,adminPrx,display,context)
{
    id_ = "::orca::Button";

    addOperation( "press",        "void press()" );
}
    
int 
ButtonProbe::loadOperationEvent( const int index, orcacm::OperationData& data )
{    
    switch ( index )
    {
    case orcaprobe::UserIndex :
        return loadPress( data );
    }
    return 1;
}

int 
ButtonProbe::loadPress( orcacm::OperationData& data )
{
    orca::ButtonPrx derivedPrx = orca::ButtonPrx::checkedCast(prx_);
    derivedPrx->press();
    return 0;
}
