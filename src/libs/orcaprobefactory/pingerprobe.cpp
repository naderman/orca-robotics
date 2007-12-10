/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>
#include <orcacm/orcacm.h>

#include "pingerprobe.h"
#include <orcaprobe/orcaprobe.h>

using namespace std;
using namespace orcaprobefactory;

PingerProbe::PingerProbe( const orca::FQInterfaceName & name, orcaprobe::IDisplay & display,
                                const orcaice::Context & context )
    : InterfaceProbe(name,display,context)
{
    id_ = "::orca::Pinger";
    
    addOperation( "ping" );
}

int 
PingerProbe::loadOperationEvent( const int index, orcacm::OperationData & data )
{    
    switch ( index )
    {
    case orcaprobe::UserIndex :
        return loadPing( data );
    }
    return 1;
}

int 
PingerProbe::loadPing( orcacm::OperationData & data )
{

    cout << "Type in hostname or IP address:"<<endl;
    cout << "==>";

    // block until get a string + Enter
    std::string hostname;
    cin >> hostname;
    if ( hostname.empty() ) {
        cout<<"hostname cannot be empty"<<endl;
        return 1;
    }

    try
    {
        orca::PingerPrx derivedPrx = orca::PingerPrx::checkedCast(prx_);
        double latencyMs = derivedPrx->ping( hostname );
        cout<<"latency = "<<latencyMs<<"ms"<<endl;
    }
    catch( const orca::OrcaException & e )
    {
        cout<<"Caught orca::OrcaException"<<endl;
        cout << e << ": " << e.what <<endl;
        return 1;
    }
    catch( const Ice::Exception & e )
    {
        cout<<"Caught Ice::Exception"<<endl;
        cout << e <<endl;
        return 1;
    }
    catch( ... )
    {
        cout << "Caught something unexpected while pinging" <<endl;
        return 1;
    }
    
    return 0;
}
