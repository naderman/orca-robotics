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

#include "binaryswitchprobe.h"
#include "displaydriver.h"

using namespace std;
using namespace probe;

BinarySwitchProbe::BinarySwitchProbe( const orca::FQInterfaceName & name, DisplayDriver & display,
                                const orcaice::Context & context )
    : InterfaceProbe(name,display,context)
{
    id_ = "::orca::BinarySwitch";
    
    operations_.push_back( "getData" );
    operations_.push_back( "setState" );
    operations_.push_back( "timedSetState" );
    operations_.push_back( "toggleState" );
    operations_.push_back( "timedToggleState" );
}

int 
BinarySwitchProbe::loadOperation( const int index )
{
    //cout<<"loading home operation "<<index<<endl;

    int ret = 1;
    
    switch ( index )
    {
    case 0 :
        ret = loadGetData();
        break;
    case 1 :
        ret = loadSetState();
        break;
    case 2 :
        ret = loadTimedSetState();
        break;
    case 3 :
        ret = loadToggleState();
        break;
    case 4 :
        ret = loadTimedToggleState();
        break;
    default :
        ret = 1;
        cout<<"unknown operation index"<<endl;
        break;
    }

    return ret;
}

int 
BinarySwitchProbe::loadGetData()
{
    orca::BinarySwitchDataPtr data;
    
    try
    {
        orca::BinarySwitchPrx derivedPrx = orca::BinarySwitchPrx::checkedCast(prx_);
        data = derivedPrx->getData();
    }
    catch( const Ice::Exception & e )
    {
        return 1;
    }

    cout<<orcaice::toString(data)<<endl;
    return 0;
}

int 
BinarySwitchProbe::loadSetState()
{
    int dev = 1;
    // ask for device number
    bool state = true;
    // ask for state

    try
    {
        orca::BinarySwitchPrx derivedPrx = orca::BinarySwitchPrx::checkedCast(prx_);
        derivedPrx->setState( dev, state );
    }
    catch( const orca::OrcaException & e )
    {
        cout << e <<endl;
        return 1;
    }
    catch( const Ice::Exception & e )
    {
        cout << e <<endl;
        return 1;
    }
    
    return 0;
}

int 
BinarySwitchProbe::loadTimedSetState()
{
    return 0;
}

int 
BinarySwitchProbe::loadToggleState()
{
    return 0;
}

int 
BinarySwitchProbe::loadTimedToggleState()
{
    return 0;
}
