/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <IceGrid/Admin.h>

#include "icegridutils.h"

using namespace std;

namespace orcaice {

bool
isRegistryReachable( const Context& context )
{
    Ice::CommunicatorPtr ic = context.communicator();
    assert( ic );

    Ice::LocatorPrx locatorPrx = ic->getDefaultLocator();
    // debug
    context.tracer().debug( "pinging "+ic->proxyToString( locatorPrx ),5 );

    try 
    {
        // ping the registry
        locatorPrx->ice_ping();
        return true;
    }
    catch( const std::exception &e )
    {
        std::stringstream ss;
        ss << "orcaice::isRegistryReachable(): caught exception: " << e.what();
        context.tracer().debug( ss.str() ,5 );
    }
    return false;
}

IceGrid::QueryPrx 
getDefaultQuery( const orcaice::Context& context )
{
    Ice::CommunicatorPtr ic = context.communicator();
    assert( ic );

    Ice::ObjectPrx locatorPrx = ic->getDefaultLocator();

    Ice::Identity locatorId = locatorPrx->ice_getIdentity();
    Ice::Identity queryId;
    queryId.category = locatorId.category;
    queryId.name = "Query";

    Ice::ObjectPrx objPrx = ic->stringToProxy( ic->identityToString( queryId ) );
            
    IceGrid::QueryPrx queryPrx;

    try {
//         objPrx->ice_ping();
//         string address = orcacm::connectionToRemoteAddress( queryPrx->ice_getConnection()->toString() );

//         std::ostringstream os;
//         os<<"Registry ping successful: "<<data.address;
//         context.tracer().debug( os.str() );

        queryPrx = IceGrid::QueryPrx::checkedCast( objPrx );
    } 
    catch ( const Ice::Exception& e ) {
        // what do we do?
        ostringstream os;
        os << "(while looking for IceGrid Query interface) :"<<e.what();
        context.tracer().warning( os.str() );
    }

    return queryPrx;
}

Ice::Identity
toAdminIdentity( const orca::FQComponentName& name )
{
    Ice::Identity adminId;
    adminId.category = name.platform + "." + name.component;
    adminId.name = "admin";

    return adminId;
}

orca::FQComponentName 
toComponentName( const Ice::Identity& adminId )
{
    orca::FQComponentName fqCName;

    string s = adminId.category;

    int posDot = s.find( '.', 0 );
    //cout<<"/ found :"<<posSlash<<endl;

    if ( posDot<0 ) { 
        // delimeter not found: return null structure
    }
    else {
        fqCName.platform = s.substr( 0, posDot );
        fqCName.component = s.substr( posDot+1, s.npos );
    }

    //cout<<"debug: "<<toString(fqCName)<<endl;
    return fqCName;
}

// std::string 
// toHomeIdentity( const orca::FQComponentName & fqCName )
// {
//     return "orca." + fqCName.platform + "." + fqCName.component + "/Home";
// }

orca::FQComponentName 
toComponentName( const Ice::ObjectPrx& homePrx )
{
// THE OLD WAY: a Home was on the component's adapter
//     string adapt = homePrx->ice_getAdapterId();
//     return orcaice::toComponentName( adapt );

// THE NEW WAY: a Home is a facet on the Admin interface
    Ice::Identity ident = homePrx->ice_getIdentity();
//     cout<<"DEBUG: ident="<<ident.category<<"/"<<ident.name<<endl;
    return orcaice::toComponentName( ident );
}

Ice::ObjectPrx
getComponentAdmin( const orcaice::Context& context, const orca::FQComponentName& component )
{    
    Ice::CommunicatorPtr ic = context.communicator();
    assert( ic );

    Ice::LocatorPrx locatorPrx = ic->getDefaultLocator();
    
    Ice::Identity adminId = toAdminIdentity( component );
    Ice::ObjectPrx adminPrx;

    try {
        adminPrx = locatorPrx->findObjectById( adminId );
    } 
    catch ( const Ice::Exception& ) {
        // what do we do?
    }

    return adminPrx;
}

Ice::ObjectProxySeq
getAllComponentAdmins( const orcaice::Context& context )
{    
    IceGrid::QueryPrx queryPrx = getDefaultQuery( context );
    
    Ice::ObjectProxySeq list;
    try {
        list = queryPrx->findAllObjectsByType( "::orca::Home" );
    } 
    catch ( const Ice::Exception& ) {
        // what do we do?
    }

    return list;
}

} // namespace
