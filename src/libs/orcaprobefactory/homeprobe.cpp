/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2006 Alexei Makarenko
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

HomeProbe::HomeProbe( const orca::FQInterfaceName & name, orcaprobe::DisplayDriver & display,
                                const orcaice::Context & context )
    : InterfaceProbe(name,display,context)
{
    id_ = "::orca::Home";
    
    addOperation( "getInterfaces", "nonmutating HomeData getInterfaces()" );
    addOperation( "getProperties", "nonmutating ComponentProperties getProperties()" );
}

int 
HomeProbe::loadOperationEvent( const int index, orcacm::OperationData & data )
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
HomeProbe::loadGetInterfaces( orcacm::OperationData & data )
{
//     cout<<"Calling operation orca::Home::getInterfaces()..."<<endl;

    orca::HomeData result;
    orcacm::ResultHeader res;
    
    try
    {
        orca::HomePrx dereivedPrx = orca::HomePrx::checkedCast(prx_);

        result = dereivedPrx->getInterfaces();
    }
    catch( const Ice::Exception & e )
    {
        stringstream ss;
        ss << e;
        res.name = "exception";
        res.text = ss.str();
        data.results.push_back( res );
        return 1;
    }

    res.name = "data";
    res.text = orcaice::toString(result);
    data.results.push_back( res );
    return 0;
}

int 
HomeProbe::loadGetProperties( orcacm::OperationData & data )
{
//     cout<<"Calling operation orca::Home::getProperties()..."<<endl;

    std::map<std::string,std::string> result;
    orcacm::ResultHeader res;
    
    try
    {
        orca::HomePrx dereivedPrx = orca::HomePrx::checkedCast(prx_);

        result = dereivedPrx->getProperties();
    }
    catch( const Ice::Exception & e )
    {
        stringstream ss;
        ss << e;
        res.name = "exception";
        res.text = ss.str();
        data.results.push_back( res );
        return 1;
    }

    stringstream ss;
    for ( map<string,string>::iterator it=result.begin(); it!=result.end(); ++it ) {
        ss<<setw(40)<<it->first<<"\t\t"<<it->second<<endl;
    }
    res.name = "data";
    res.text = ss.str();
    data.results.push_back( res );

    return 0;
}
