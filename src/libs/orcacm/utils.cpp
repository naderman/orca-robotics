/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <Ice/Ice.h>
#include <IceGrid/Admin.h>
#include <IceGrid/Query.h>
#include <orcaice/orcaice.h>
#include <orcaice/icegridutils.h>
#include <orcacm/parseutils.h>
#include <orca/home.h>

#include "utils.h"

using namespace std;

namespace orcacm
{

bool
pingObject( const orcaice::Context& context, const std::string& objectId )
{
    try {
        Ice::ObjectPrx objPrx = context.communicator()->stringToProxy( objectId );
        objPrx->ice_ping();
    }
    catch( const Ice::Exception& )
    {
        return false;
    }
    return true;
}

bool
pingAdminObject( const orcaice::Context& context, const std::string& adminId, const std::string& facetName )
{
    try {
        Ice::ObjectPrx objPrx = context.communicator()->stringToProxy( adminId )->ice_facet( facetName );
        objPrx->ice_ping();
    }
    catch( const Ice::Exception& )
    {
        return false;
    }
    return true;
}

RegistryHomeData
getRegistryHomeData( const orcaice::Context& context, const std::string& locatorString )
{
    RegistryHomeData data;

    data.locatorString = locatorString;
    
    IceGrid::QueryPrx queryPrx = orcaice::getDefaultQuery( context );
    
    Ice::ObjectProxySeq list;
    try {
        queryPrx->ice_ping();
        data.address = orcacm::connectionToRemoteAddress( queryPrx->ice_getConnection()->toString() );
        context.tracer().debug( "(while retrieving home data) registry ping successful: "+data.address );

        data.isReachable = true;
        IceGrid::QueryPrx query = IceGrid::QueryPrx::checkedCast( queryPrx );

        //
        // get list of homes
        //
        list = query->findAllObjectsByType( "::orca::Home" );

        std::ostringstream os;
        os<<"(while retrieving home data) found "<<list.size()<<" homes";
        context.tracer().debug( os.str() );
    } 
    catch ( const Ice::Exception& ) {
        data.isReachable = false;
        return data;
    }

    // add more information
    HomeHeader home;
    for ( unsigned int i=0; i<list.size(); ++i ) 
    {
        home.proxy = list[i];
        // assume it's reachable
        home.isReachable = true;
        
        data.homes.push_back( home );
    }

    return data;
}

void pingHomeObjects( RegistryHomeData& data, const orcaice::Context& context, 
                 gbxutilacfr::Stoppable* activity )
{
//     context.tracer().debug( "Pinging  );
    HomeHeader home;
    cout<<endl;
    for ( unsigned int i=0; i<data.homes.size(); ++i ) 
    {
        // indicate that we started pinging the next home
        cout<<"."<<flush;

        // assume it's reachable
        data.homes[i].isReachable = true;
        
        try {
            data.homes[i].proxy->ice_ping();
        }
        catch( const Ice::Exception& )
        {
            data.homes[i].isReachable = false;
        }

        data.homes[i].address = "not implemented";

        // indicate that we finished pinging this home
        cout<<"|"<<flush;
    }
cout<<endl;
}

ComponentData
getComponentData( const orcaice::Context& context, const std::string& adapterId )
{
    return getComponentData( context, orcaice::toComponentName( adapterId ) );
}

// this is the old-old way: Home was just home@platform/component
ComponentData
getComponentData( const orcaice::Context& context, const orca::FQComponentName& component )
{
    ComponentData data;
    data.name = component;
    
    orca::HomeData homeData;
    
    try
    {
        Ice::ObjectPrx ohome = context.communicator()->stringToProxy( "home@"+orcaice::toString(data.name) );
        orca::HomePrx home = orca::HomePrx::checkedCast(ohome);

        homeData = home->getData();
        data.isReachable = true;

        // we must've connected, otherwise we'd have an exception
        try
        {
            Ice::ConnectionPtr conn = ohome->ice_getConnection();
            data.address = orcacm::connectionToRemoteAddress( conn->toString() );
            //std::cout<<"remote address : ["<<homeData.address<<"]"<<std::endl;
        }
        catch( const Ice::CollocationOptimizationException& )
        {
            data.address = "collocated";
        }
    }
    catch( const Ice::Exception& )
    {
        data.isReachable = false;
        return data;
    }
    
    // look up provided interface types
    ProvidesHeader pface;
    for ( unsigned int j=0; j<homeData.comp.provides.size(); ++j ) {   
        pface = getProvidesHeader( context,
                    orcaice::toInterfaceName( homeData.comp.provides[j].name+"@"+orcaice::toString(data.name) ) );
        data.provides.push_back( pface );
    }

    // for now, we don't actually try to lookup required interfaces
    RequiresHeader rface;
    for ( unsigned int j=0; j<homeData.comp.requires.size(); ++j ) {
        rface.name = homeData.comp.requires[j].name;
        rface.id = homeData.comp.requires[j].id;
        rface.isReachable = false;
        data.requires.push_back( rface );
    }

    return data;
}

ComponentData
getComponentHomeData( const orcaice::Context& context, const Ice::ObjectPrx& ohome )
{
    ComponentData data;
    data.adminPrx = ohome;
    data.name = orcaice::toComponentName( ohome );

    orca::HomeData homeData;

    try
    {
        orca::HomePrx home = orca::HomePrx::checkedCast(ohome);

        homeData = home->getData();
        // debug
//         cout<<orcaice::toString(homeData)<<endl;
        data.isReachable = true;

        // we must've connected, otherwise we'd have an exception
        try
        {
            Ice::ConnectionPtr conn = ohome->ice_getConnection();
            data.address = orcacm::connectionToRemoteAddress( conn->toString() );
            //std::cout<<"remote address : ["<<homeData.address<<"]"<<std::endl;
        }
        catch( const Ice::CollocationOptimizationException& )
        {
            data.address = "collocated";
        }
    }
    catch( const Ice::Exception& )
    {
        data.isReachable = false;
        return data;
    }

    // look up provided interface types
    ProvidesHeader pface;
    for ( unsigned int j=0; j<homeData.comp.provides.size(); ++j )
    {
        // debug
        //cout<<"looking up interface "<<homeData.comp.provides[j].name<<" on "<<orcaice::toString(data.name)<<endl;
        try {
            orca::FQInterfaceName fqName;
            fqName.platform = data.name.platform;
            fqName.component = data.name.component;
            fqName.iface = homeData.comp.provides[j].name;

            // remote call!
            if ( homeData.comp.provides[j].id  == "::orca::Tracer" ||    
                 homeData.comp.provides[j].id  == "::orca::Status" || 
                 homeData.comp.provides[j].id  == "::orca::Home" )  
            {
                pface = getAdminProvidesHeader( ohome, fqName );
            }
            else
            {
                pface = getProvidesHeader( context, fqName );
            }
        }
        catch ( std::exception &e )
        {
            cout << __func__ << ": Failed to lookup interface "<<homeData.comp.provides[j].name
                 <<" on "<<orcaice::toString(data.name)<<": "<<e.what()<<endl;
            pface.isReachable = false;
        }
        data.provides.push_back( pface );
    }

    // for now, we don't actually try to lookup required interfaces
    RequiresHeader rface;
    for ( unsigned int j=0; j<homeData.comp.requires.size(); ++j ) {
        rface.name = homeData.comp.requires[j].name;
        rface.id = homeData.comp.requires[j].id;
        rface.isReachable = false;
        data.requires.push_back( rface );
    }

    return data;
}

ProvidesHeader
getAdminProvidesHeader( const Ice::ObjectPrx& homePrx, const orca::FQInterfaceName& fqName )
{
// cout<<"DEBUG: getAdminProvidesHeader() fqName.iface="<< fqName.iface << endl;
    ProvidesHeader header;
    header.name = fqName.iface;
    
    // orcaice::createAdminInterface() registers interfaces with Home under the same name as the
    // Admin facet.
    Ice::ObjectPrx obj = homePrx->ice_facet( fqName.iface );
   
    //cout<<"looking up proxy "<<obj<<endl;
    try
    {
        header.id = obj->ice_id();
        header.isReachable = true;
    }
    catch( const Ice::ObjectNotExistException& )
    {
        //cout<<e<<endl;
        //cout << "failed to reach "<< header.name << endl;
        header.id = "Ice::ObjectNotExistException";
        header.isReachable = false;
    }
    catch( const Ice::Exception& e )
    {
        cout<<e<<endl;
        cout << "failed to lookup ID for "<< header.name << endl;
        header.id = "unknown";
        // does this make sense?
        header.isReachable = true;
    }
    return header;
}

ProvidesHeader
getProvidesHeader( const orcaice::Context& context, const orca::FQInterfaceName& fqName )
{
    ProvidesHeader header;
    header.name = fqName.iface;
    
    Ice::ObjectPrx obj = context.communicator()->stringToProxy( orcaice::toString(fqName) );
    try
    {
        header.id = obj->ice_id();
        header.isReachable = true;
    }
    catch( const Ice::ObjectNotExistException& )
    {
        //cout<<e<<endl;
        //cout << "failed to reach "<< header.name << endl;
        header.id = "Ice::ObjectNotExistException";
        header.isReachable = false;
    }
    catch( const Ice::Exception& e )
    {
        cout<<e<<endl;
        cout << "failed to lookup ID for "<< header.name << endl;
        header.id = "unknown";
        // does this make sense?
        header.isReachable = true;
    }
    return header;
}

RequiresHeader
getRequiresHeader( const orcaice::Context& context, const orca::FQInterfaceName& fqName )
{
    RequiresHeader header;
    header.name = fqName;
    try
    {
        Ice::ObjectPrx obj = context.communicator()->stringToProxy( orcaice::toString(fqName) );
        header.id = obj->ice_id();
    }
    catch( const Ice::Exception& )
    {
        cout << "failed to lookup ID for "<< orcaice::toString(header.name) << endl;
        header.id = "unknown";
    }
    return header;
}

RegistryHierarchicalData1
home2hierarch1( const RegistryHomeData& registryHomeData )
{
    RegistryHierarchicalData1 hierData;
    
    hierData.locatorString = registryHomeData.locatorString;
    hierData.address = registryHomeData.address;
    hierData.isReachable = registryHomeData.isReachable;

    orca::FQComponentName compName;
    PlatformHeader platformHeader;
    bool isDuplicate;
    for ( unsigned int i=0; i<registryHomeData.homes.size(); ++i ) {
        isDuplicate = false;

        // local call
        compName = orcaice::toComponentName( registryHomeData.homes[i].proxy );
        
        // eliminate duplicates
        for ( unsigned int j=0; j<hierData.platforms.size(); ++j ) {
//             cout<<"DEBUG: new="<<compName.platform<<" exist="<<hierData.platforms[j].name<<endl;
            if ( compName.platform == hierData.platforms[j].name ) {
                isDuplicate = true;
                break;
            }
        }
        if ( !isDuplicate ) {
            platformHeader.name = compName.platform;
            hierData.platforms.push_back( platformHeader );
        }
    }
//     cout<<"DEBUG: processed "<<registryHomeData.homes.size()<<" homes into "<<hierData.platforms.size()<<" platforms"<<endl;
    return hierData;
}

RegistryHierarchicalData2
home2hierarch2( const RegistryHomeData& registryHomeData, const PlatformHeader& platform, 
                bool tryToPing, int tracePing )
{
    RegistryHierarchicalData2 hierData;

    hierData.locatorString = registryHomeData.locatorString;
    hierData.address = registryHomeData.address;
    hierData.isReachable = registryHomeData.isReachable;
    hierData.platform = platform;

    std::string adapt;
    orca::FQComponentName compName;

    if ( tracePing )
        cout<<endl;
    for ( unsigned int i=0; i<registryHomeData.homes.size(); ++i ) {

        // local call
        compName = orcaice::toComponentName( registryHomeData.homes[i].proxy );
        
        // select home headers on the given platform
        if ( compName.platform == platform.name ) {

            HomeHeader homeHeader = registryHomeData.homes[i];
            // first, we are optimistic
            homeHeader.isReachable = true;
            // ping this component's Home interface, if requested
            if ( tryToPing ) {

                switch ( tracePing ) 
                {
                case 1 :
                    cout<<"."<<flush;
                    break;
                case 2 :
                    cout<<"pinging "<<homeHeader.proxy->ice_toString()<<flush;
                    break;
                default :
                    break;
                }

                try {

                    homeHeader.proxy->ice_ping();
                }
                catch( const Ice::Exception& )
                {
                    homeHeader.isReachable = false;
                }

                switch ( tracePing ) 
                {
                case 1 :
                    cout<<"|";
                    break;
                case 2 :
                    cout<<endl;
                    break;
                default :
                    break;
                }

                homeHeader.address = "not implemented";
            }
            // store it
            hierData.homes.push_back( homeHeader );
        }
    }
    if ( tracePing )
        cout<<endl;
    cout<<"DEBUG: filtered "<<registryHomeData.homes.size()<<" homes into "<<hierData.homes.size()<<" on platform "<<hierData.platform.name<<endl;
 
    return hierData;
}

} // namespace
