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
#include <orcaobj/orcaobj.h>
#include <orcacm/orcacm.h>

#include "binaryswitchprobe.h"
#include <orcaprobe/orcaprobe.h>

using namespace std;
using namespace orcaprobefactory;

BinarySwitchProbe::BinarySwitchProbe( const orca::FQInterfaceName & name, orcaprobe::IDisplay & display,
                                const orcaice::Context & context )
    : InterfaceProbe(name,display,context)
{
    id_ = "::orca::BinarySwitch";
    
    addOperation( "getData" );
    addOperation( "setState" );
    addOperation( "timedSetState" );
    addOperation( "toggleState" );
    addOperation( "timedToggleState" );
}

int 
BinarySwitchProbe::loadOperationEvent( const int index, orcacm::OperationData & data )
{    
    switch ( index )
    {
    case orcaprobe::UserIndex :
        return loadGetData( data );
    case orcaprobe::UserIndex+1 :
        return loadSetState( data );
    case orcaprobe::UserIndex+2 :
        return loadTimedSetState( data );
    case orcaprobe::UserIndex+3 :
        return loadToggleState( data );
    case orcaprobe::UserIndex+4 :
        return loadTimedToggleState( data );
    }
    return 1;
}

int 
BinarySwitchProbe::loadGetData( orcacm::OperationData & data )
{
    orca::BinarySwitchData result;
    
    try
    {
        orca::BinarySwitchPrx derivedPrx = orca::BinarySwitchPrx::checkedCast(prx_);
        result = derivedPrx->getData();
    }
    catch( const Ice::Exception & e )
    {
        return 1;
    }

    cout<<orcaobj::toString(result)<<endl;
    return 0;
}

int 
BinarySwitchProbe::loadSetState( orcacm::OperationData & data )
{
    // first get a list of devices
    orca::BinarySwitchData result;
    
    try
    {
        orca::BinarySwitchPrx derivedPrx = orca::BinarySwitchPrx::checkedCast(prx_);
        result = derivedPrx->getData();
    }
    catch( const Ice::Exception & e )
    {
        cout<<e<<endl;
        return 1;
    }
    catch( const std::exception & e )
    {
        cout<<e.what()<<endl;
        return 1;
    }

    cout << "Select device from the list:"<<endl;
    for ( unsigned int i=0; i<result.devices.size(); ++i ) {
        cout<<i<<"\t"<<result.devices[i].name<<endl;
    }
    cout << "==>";

    // block until get a character + Enter
    int dev = 0;
    cin >> dev;
    if ( dev<0 ) {
        cout<<"device number cannot be negative"<<endl;
        return 1;
    }

    cout << "Enter desired state (0/1):"<<endl;
    cout << "==>";
    int state = 1;
    cin >> state;

    try
    {
        orca::BinarySwitchPrx derivedPrx = orca::BinarySwitchPrx::checkedCast(prx_);
        derivedPrx->setState( dev, (bool)state );
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
BinarySwitchProbe::loadTimedSetState( orcacm::OperationData & data )
{
    return 0;
}

int 
BinarySwitchProbe::loadToggleState( orcacm::OperationData & data )
{
    return 0;
}

int 
BinarySwitchProbe::loadTimedToggleState( orcacm::OperationData & data )
{
    return 0;
}
