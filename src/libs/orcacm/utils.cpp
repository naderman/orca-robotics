/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <IceGrid/Admin.h>
#include <IceGrid/Query.h>
#include <orcaice/orcaice.h>
#include <orcamisc/orcamisc.h>  // for connectionToRemoteAddress()

#include "utils.h"

using namespace std;

namespace orcacm
{

bool
pingComponent( const orcaice::Context & context, const std::string & adapterId )
{
    try {
        Ice::ObjectPrx obj = context.communicator()->stringToProxy( "home@"+adapterId );

        // debug
        //std::cout<<"pinging "<<context_.communicator()->proxyToString( base )<<std::endl;
        
        obj->ice_ping();

        //cout<<endl<<"Ping successful."<<endl;
        //cout<<"Proxy\t\t[ "<<base->ice_toString()<<" ]"<<endl;

    }
    catch( const Ice::Exception & )
    {
        return false;
    }
    
    return true;
}

RegistryData
getRegistryData( const orcaice::Context & context, const std::string & locatorString, bool pingAdapters )
{
    RegistryData data;
    
    data.locatorString = locatorString;
    
    Ice::ObjectPrx adminPrx = context.communicator()->stringToProxy(
            orcamisc::stringToIceGridInstanceName(locatorString)+"/Admin");
    
    try {
        adminPrx->ice_ping();
        data.adminAddress = orcamisc::connectionToRemoteAddress( adminPrx->ice_getConnection()->toString() );
        data.isReachable = true;

        std::ostringstream os;
        os<<"Ping successful: "<<data.adminAddress;
        context.tracer()->debug( os.str() );
    } catch ( const Ice::Exception & ) {
        data.isReachable = false;
        return data;
    }
    
    IceGrid::AdminPrx admin = IceGrid::AdminPrx::checkedCast( adminPrx );
    
    //
    // get adapter list
    //
    Ice::StringSeq list = admin->getAllAdapterIds();

    // add more information
    ComponentHeader comp;
    for ( unsigned int i=0; i<list.size(); ++i ) {
        comp.name = orcaice::toComponentName( list[i] );
        
        // ping each component's Home interface, if requested
        if ( pingAdapters ) {
            comp.isReachable = orcacm::pingComponent( context, list[i] );
            comp.address = "not implemented";
        }
        
        data.adapters.push_back( comp );
    }

    std::ostringstream os;
    os<<"Retrieved list of "<<data.adapters.size()<<" adapters";
    context.tracer()->debug( os.str() );

    return data;
}

RegistryHomeData
getRegistryHomeData( const orcaice::Context & context, const std::string & locatorString, bool pingAdapters )
{
    RegistryHomeData data;

    data.locatorString = locatorString;
    
    Ice::ObjectPrx queryPrx = context.communicator()->stringToProxy(
            orcamisc::stringToIceGridInstanceName(locatorString)+"/Query");
    
    try {
        queryPrx->ice_ping();
        data.adminAddress = orcamisc::connectionToRemoteAddress( queryPrx->ice_getConnection()->toString() );
        data.isReachable = true;

        std::ostringstream os;
        os<<"Ping successful: "<<data.adminAddress;
        context.tracer()->debug( os.str() );
    } catch ( const Ice::Exception & ) {
        data.isReachable = false;
        return data;
    }
    
    IceGrid::QueryPrx query = IceGrid::QueryPrx::checkedCast( queryPrx );

    //
    // get list of homes
    //
    data.homes = query->findAllObjectsByType( "::orca::Home" );

    return data;
}

ComponentData
getComponentData( const orcaice::Context & context, const std::string & adapterId )
{
    return getComponentData( context, orcaice::toComponentName( adapterId ) );
}

ComponentData
getComponentData( const orcaice::Context & context, const orca::FQComponentName & component )
{
    ComponentData data;
    data.name = component;
    
    orca::HomeDataPtr homeData;
    
    try
    {
        Ice::ObjectPrx ohome = context.communicator()->stringToProxy( "home@"+orcaice::toString(data.name) );
        orca::HomePrx home = orca::HomePrx::checkedCast(ohome);

        homeData = home->getInterfaces();
        data.isReachable = true;

        // copy time since activation
        data.timeUp = homeData->timeUp;
        // debug
//         cout<<"time up :"<<data.timeUp<<"s"<<endl;

        // we must've connected, otherwise we'd have an exception
        try
        {
            Ice::ConnectionPtr conn = ohome->ice_getConnection();
            data.address = orcamisc::connectionToRemoteAddress( conn->toString() );
            //std::cout<<"remote address : ["<<homeData->address<<"]"<<std::endl;
        }
        catch( const Ice::CollocationOptimizationException & )
        {
            data.address = "collocated";
        }
    }
    catch( const Ice::Exception & )
    {
        data.isReachable = false;
        data.timeUp = 0;
        return data;
    }
    
    // look up provided interface types
    ProvidesHeader pface;
    for ( unsigned int j=0; j<homeData->comp.provides.size(); ++j ) {   
        pface = getProvidesHeader( context,
                    orcaice::toInterfaceName( homeData->comp.provides[j].name+"@"+orcaice::toString(data.name) ) );
        data.provides.push_back( pface );
    }

    // for now, we don't actually try to lookup required interfaces
    RequiresHeader rface;
    for ( unsigned int j=0; j<homeData->comp.requires.size(); ++j ) {
        rface.name = homeData->comp.requires[j].name;
        rface.id = homeData->comp.requires[j].id;
        rface.isReachable = false;
        data.requires.push_back( rface );
    }

    return data;
}

ComponentData
getComponentHomeData( const orcaice::Context & context, const Ice::ObjectPrx & ohome )
{
    ComponentData data;
    data.name = orcaice::toComponentName( ohome->ice_getAdapterId() );

    orca::HomeDataPtr homeData;

    try
    {
        orca::HomePrx home = orca::HomePrx::checkedCast(ohome);

        homeData = home->getInterfaces();
        // debug
//         cout<<orcaice::toString(homeData)<<endl;
        data.isReachable = true;

        // copy time since activation
        data.timeUp = homeData->timeUp;
        // debug
//         cout<<"time up :"<<data.timeUp<<"s"<<endl;

        // we must've connected, otherwise we'd have an exception
        try
        {
            Ice::ConnectionPtr conn = ohome->ice_getConnection();
            data.address = orcamisc::connectionToRemoteAddress( conn->toString() );
            //std::cout<<"remote address : ["<<homeData->address<<"]"<<std::endl;
        }
        catch( const Ice::CollocationOptimizationException & )
        {
            data.address = "collocated";
        }
    }
    catch( const Ice::Exception & )
    {
        data.isReachable = false;
        data.timeUp = 0;
        return data;
    }

    // look up provided interface types
    ProvidesHeader pface;
    for ( unsigned int j=0; j<homeData->comp.provides.size(); ++j ) {   
        // debug
//         cout<<"looking up interface "<<homeData->comp.provides[j].name<<" on "<<orcaice::toString(data.name)<<endl;
        orca::FQInterfaceName fqName;
        fqName.platform = data.name.platform;
        fqName.component = data.name.component;
        fqName.iface = homeData->comp.provides[j].name;
        pface = getProvidesHeader( context, fqName );
        data.provides.push_back( pface );
    }

    // for now, we don't actually try to lookup required interfaces
    RequiresHeader rface;
    for ( unsigned int j=0; j<homeData->comp.requires.size(); ++j ) {
        rface.name = homeData->comp.requires[j].name;
        rface.id = homeData->comp.requires[j].id;
        rface.isReachable = false;
        data.requires.push_back( rface );
    }

    return data;
}

ProvidesHeader
getProvidesHeader( const orcaice::Context & context, const orca::FQInterfaceName & fqName )
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
    catch( const Ice::ObjectNotExistException & )
    {
        //cout<<e<<endl;
        //cout << "failed to reach "<< header.name << endl;
        header.id = "Ice::ObjectNotExistException";
        header.isReachable = false;
    }
    catch( const Ice::Exception & e )
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
getRequiresHeader( const orcaice::Context & context, const orca::FQInterfaceName & fqName )
{
    RequiresHeader header;
    header.name = fqName;
    try
    {
        Ice::ObjectPrx obj = context.communicator()->stringToProxy( orcaice::toString(fqName) );
        header.id = obj->ice_id();
    }
    catch( const Ice::Exception & )
    {
        cout << "failed to lookup ID for "<< orcaice::toString(header.name) << endl;
        header.id = "unknown";
    }
    return header;
}

} // namespace
