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
HomeProbe::loadOperation( const int index )
{
    //cout<<"loading dereivedPrx operation "<<index<<endl;

    int ret = 1;
    
    switch ( index )
    {
    case 0 :
        ret = loadGetInterfaces();
        break;
    case 1 :
        ret = loadGetProperties();
        break;
    default :
        ret = 1;
        cout<<"unknown operation index"<<endl;
        break;
    }

    return ret;
}

int 
HomeProbe::loadGetInterfaces()
{
    cout<<"Calling operation orca::Home::getInterfaces()..."<<endl;

    orca::HomeDataPtr homeData;
    
    try
    {
        orca::HomePrx dereivedPrx = orca::HomePrx::checkedCast(prx_);

        homeData = dereivedPrx->getInterfaces();
    }
    catch( const Ice::Exception & e )
    {
        return 1;
    }

    cout<<orcaice::toString(homeData)<<endl;
    return 0;
}

int 
HomeProbe::loadGetProperties()
{
    cout<<"Calling operation orca::Home::getProperties()..."<<endl;

    std::map<std::string,std::string> props;
    
    try
    {
        orca::HomePrx dereivedPrx = orca::HomePrx::checkedCast(prx_);

        props = dereivedPrx->getProperties();
    }
    catch( const Ice::Exception & e )
    {
        return 1;
    }

    for ( map<string,string>::iterator it=props.begin(); it!=props.end(); ++it ) {
        cout<<setw(40)<<it->first<<"\t\t"<<it->second<<endl;
    }

    return 0;
}
