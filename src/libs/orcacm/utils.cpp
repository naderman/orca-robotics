/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <IceGrid/Admin.h>
#include <IceGrid/Query.h>
#include <orcaice/orcaice.h>
#include <orcacm/parseutils.h>

#include "utils.h"

using namespace std;

namespace orcacm
{

bool
pingObject( const orcaice::Context& context, const std::string& objectId )
{
    try {
        Ice::ObjectPrx obj = context.communicator()->stringToProxy( objectId );
        
        obj->ice_ping();

        //cout<<endl<<"Ping successful."<<endl;
        //cout<<"Proxy\t\t[ "<<base->ice_toString()<<" ]"<<endl;

    }
    catch( const Ice::Exception& )
    {
        return false;
    }
    
    return true;
}

RegistryHomeData
getRegistryHomeData( const orcaice::Context& context, const std::string& locatorString, bool tryToPing )
{
    RegistryHomeData data;

    data.locatorString = locatorString;
    
    Ice::ObjectPrx queryPrx = context.communicator()->stringToProxy(
            orcacm::stringToIceGridInstanceName(locatorString)+"/Query");
    
    Ice::ObjectProxySeq list;
    try {
        queryPrx->ice_ping();
        data.address = orcacm::connectionToRemoteAddress( queryPrx->ice_getConnection()->toString() );
        data.isReachable = true;

        std::ostringstream os;
        os<<"Registry ping successful: "<<data.address;
        context.tracer()->debug( os.str() );

        IceGrid::QueryPrx query = IceGrid::QueryPrx::checkedCast( queryPrx );

        //
        // get list of homes
        //
        list = query->findAllObjectsByType( "::orca::Home" );
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
        
        // ping each component's Home interface, if requested
        if ( tryToPing ) {
            try {
                home.proxy->ice_ping();
            }
            catch( const Ice::Exception& )
            {
                home.isReachable = false;
            }
            home.address = "not implemented";
        }
        
        data.homes.push_back( home );
    }

    return data;
}

ComponentData
getComponentData( const orcaice::Context& context, const std::string& adapterId )
{
    return getComponentData( context, orcaice::toComponentName( adapterId ) );
}

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

        homeData = home->getInterfaces();
        data.isReachable = true;

        // copy time since activation
        data.timeUp = homeData.timeUp;
        // debug
//         cout<<"time up :"<<data.timeUp<<"s"<<endl;

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
        data.timeUp = 0;
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
    data.name = orcaice::toComponentName( ohome->ice_getAdapterId() );

    orca::HomeData homeData;

    try
    {
        orca::HomePrx home = orca::HomePrx::checkedCast(ohome);

        homeData = home->getInterfaces();
        // debug
//         cout<<orcaice::toString(homeData)<<endl;
        data.isReachable = true;

        // copy time since activation
        data.timeUp = homeData.timeUp;
        // debug
//         cout<<"time up :"<<data.timeUp<<"s"<<endl;

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
        data.timeUp = 0;
        return data;
    }

    // look up provided interface types
    ProvidesHeader pface;
    for ( unsigned int j=0; j<homeData.comp.provides.size(); ++j ) {   
        // debug
//         cout<<"looking up interface "<<homeData.comp.provides[j].name<<" on "<<orcaice::toString(data.name)<<endl;
        orca::FQInterfaceName fqName;
        fqName.platform = data.name.platform;
        fqName.component = data.name.component;
        fqName.iface = homeData.comp.provides[j].name;
        pface = getProvidesHeader( context, fqName );
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
getProvidesHeader( const orcaice::Context& context, const orca::FQInterfaceName& fqName )
{
    ProvidesHeader header;
    header.name = fqName.iface;
    
    Ice::ObjectPrx obj = context.communicator()->stringToProxy( orcaice::toString(fqName) );
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

    std::string adapt;
    orca::FQComponentName compName;
    PlatformHeader platformHeader;
    bool isDuplicate;
    for ( unsigned int i=0; i<registryHomeData.homes.size(); ++i ) {
        isDuplicate = false;
        // local call
        adapt = registryHomeData.homes[i].proxy->ice_getAdapterId();
        compName = orcaice::toComponentName( adapt );
        
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
home2hierarch2( const RegistryHomeData& registryHomeData, const PlatformHeader& platform, bool tryToPing )
{
    RegistryHierarchicalData2 hierData;

    hierData.locatorString = registryHomeData.locatorString;
    hierData.address = registryHomeData.address;
    hierData.isReachable = registryHomeData.isReachable;
    hierData.platform = platform;

    std::string adapt;
    orca::FQComponentName compName;

    for ( unsigned int i=0; i<registryHomeData.homes.size(); ++i ) {
        // local call
        adapt = registryHomeData.homes[i].proxy->ice_getAdapterId();
        compName = orcaice::toComponentName( adapt );
        
        // select home headers on the given platform
        if ( compName.platform == platform.name ) {

            HomeHeader homeHeader = registryHomeData.homes[i];
            // first, we are optimistic
            homeHeader.isReachable = true;
            // ping each component's Home interface, if requested
            if ( tryToPing ) {
                try {
                    homeHeader.proxy->ice_ping();
                }
                catch( const Ice::Exception& )
                {
                    homeHeader.isReachable = false;
                }
                homeHeader.address = "not implemented";
            }
            // store it
            hierData.homes.push_back( homeHeader );
        }
    }
    cout<<"DEBUG: filtered "<<registryHomeData.homes.size()<<" homes into "<<hierData.homes.size()<<" on platform "<<hierData.platform.name<<endl;
 
    return hierData;
}

} // namespace
