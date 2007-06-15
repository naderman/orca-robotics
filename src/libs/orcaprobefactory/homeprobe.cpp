/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <iomanip>    // for setw()
#include <orcaice/orcaice.h>
#include <orcacm/orcacm.h>

#include "homeprobe.h"
#include <orcaprobe/orcaprobe.h>

using namespace std;
using namespace orcaprobefactory;

HomeProbe::HomeProbe( const orca::FQInterfaceName& name, orcaprobe::IDisplay& display,
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
    orca::HomeData result;   
    try
    {
        orca::HomePrx dereivedPrx = orca::HomePrx::checkedCast(prx_);
        result = dereivedPrx->getInterfaces();
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
HomeProbe::loadGetProperties( orcacm::OperationData& data )
{
    std::map<std::string,std::string> result;
    try
    {
        orca::HomePrx dereivedPrx = orca::HomePrx::checkedCast(prx_);
        result = dereivedPrx->getProperties();
        stringstream ss;
        for ( map<string,string>::iterator it=result.begin(); it!=result.end(); ++it ) {
            ss<<setw(40)<<it->first<<"\t\t"<<it->second<<endl;
        }
        orcaprobe::reportResult( data, "data", ss.str() );
    }
    catch( const Ice::Exception& e )
    {
        stringstream ss;
        ss<<e<<endl;
        orcaprobe::reportException( data, ss.str() );
    }
    return 0;
}
