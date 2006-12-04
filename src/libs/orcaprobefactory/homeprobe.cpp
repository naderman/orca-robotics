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
    
    operations_.push_back( "getInterfaces" );
    operations_.push_back( "getProperties" );
}

int 
HomeProbe::loadOperation( const int index, orcacm::OperationData & data )
{
    fillOperationData( index, data );

    int ret = 1;
    
    switch ( index )
    {
    case 0 :
        ret = loadGetInterfaces( data );
        break;
    case 1 :
        ret = loadGetProperties( data );
        break;
    default :
        ret = 1;
        cout<<"unknown operation index"<<endl;
        break;
    }

    return ret;
}

int 
HomeProbe::loadGetInterfaces( orcacm::OperationData & data )
{
    cout<<"Calling operation orca::Home::getInterfaces()..."<<endl;

    orca::HomeDataPtr result;
    
    try
    {
        orca::HomePrx dereivedPrx = orca::HomePrx::checkedCast(prx_);

        result = dereivedPrx->getInterfaces();
    }
    catch( const Ice::Exception & e )
    {
        stringstream ss;
        ss << e;
        data.result = ss.str();
        return 1;
    }

    data.result = orcaice::toString(result);
    return 0;
}

int 
HomeProbe::loadGetProperties( orcacm::OperationData & data )
{
    cout<<"Calling operation orca::Home::getProperties()..."<<endl;

    std::map<std::string,std::string> result;
    
    try
    {
        orca::HomePrx dereivedPrx = orca::HomePrx::checkedCast(prx_);

        result = dereivedPrx->getProperties();
    }
    catch( const Ice::Exception & e )
    {
        stringstream ss;
        ss << e;
        data.result = ss.str();
        return 1;
    }

    stringstream ss;
    for ( map<string,string>::iterator it=result.begin(); it!=result.end(); ++it ) {
        ss<<setw(40)<<it->first<<"\t\t"<<it->second<<endl;
    }
    data.result = ss.str();

    return 0;
}
