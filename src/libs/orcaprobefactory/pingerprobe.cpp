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

        stringstream ss;
        ss<<latencyMs<<"ms"<<endl;
        orcaprobe::reportResult( data, "latency", ss.str() );
    }
    catch( const orca::OrcaException & e )
    {
        stringstream ss;
        ss<< e << ": " << e.what <<endl;
        orcaprobe::reportException( data, ss.str() );
    }
    catch( const Ice::Exception & e )
    {
        stringstream ss;
        ss<< e <<endl;
        orcaprobe::reportException( data, ss.str() );
    }
    return 0;
}
