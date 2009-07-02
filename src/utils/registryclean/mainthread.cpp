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
#include <IceGrid/Registry.h>
#include <orcaice/orcaice.h>
#include <orcaice/icegridutils.h>
#include <orcacm/orcacm.h>

#include "mainthread.h"

using namespace std;
using namespace registryclean;

MainThread::MainThread( const orcaice::Context &context ) : 
    orcaice::SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    context_(context)
{
}

void
MainThread::work()
{
    // we provide no interfaces, so we don't activate the adapter
    cout<<"default locator (refresh) :"<<context_.communicator()->getDefaultLocator()->ice_toString()<<endl;
    
    std::string locatorString = context_.communicator()->getDefaultLocator()->ice_toString();
    std::string instanceName = orcacm::stringToIceGridInstanceName(locatorString);

    Ice::ObjectPrx base = context_.communicator()->stringToProxy( instanceName + "/Registry" );

    IceGrid::AdminPrx adminPrx;

    try
    {
        // Open an admin session with the registry
        IceGrid::RegistryPrx registry = IceGrid::RegistryPrx::checkedCast(base);
        // This assumes no access control
        std::string username = "no-access-control-assumed";
        std::string password = "no-access-control-assumed";
        IceGrid::AdminSessionPrx adminSession = registry->createAdminSession( username, password );
        
        adminPrx = adminSession->getAdmin();
        adminPrx->ice_ping();
        std::string adminAddress = orcacm::connectionToRemoteAddress( adminPrx->ice_getConnection()->toString() );
        cout<<"Ping successful: "<<adminAddress<<endl;
    }
    catch ( const Ice::Exception & e )
    {
        context_.tracer().error( "Registry '"+locatorString+"' is unreachable" );
        // nothing else to do
        context_.shutdown();
    }
    
    //
    // Well-known objects
    //
    context_.tracer().info( "Purging well-known objects." );
    purgeObjects( adminPrx );

    //
    // Adapters
    //
    context_.tracer().info( "Purging adapters." );
    purgeAdapters( adminPrx );

    // we are done
    context_.shutdown();
}


void
MainThread::purgeObjects( const IceGrid::AdminPrx& adminPrx )
{
    std::string locatorString = context_.communicator()->getDefaultLocator()->ice_toString();

    //
    // Remote call to Registry
    //
    context_.tracer().info( "Getting information about Home objects from the Registry." );
    orcacm::RegistryHomeData regData;
    regData = orcacm::getRegistryHomeData( context_, locatorString );

    if ( !regData.isReachable ) {
        context_.tracer().error( "Registry '"+locatorString+"' is unreachable" );
        // nothing else we can do
        context_.shutdown();
    }

    stringstream ss;
    ss<<"Retrieved list of "<<regData.homes.size()<<" home objects";
    context_.tracer().info( ss.str() );

    //
    // Remote calls to individual home objects
    // TODO: use thread pool!
    //
    orcacm::pingHomeObjects( regData, context_, this );

    int aliveCount = 0;
    int deadCount = 0;
    int failedCount = 0;
        
    context_.tracer().info( "Starting to purge Home objects." );
    for ( unsigned int i=0; i<regData.homes.size(); ++i ) 
    {    
        // ping each component's Home interface
        if ( regData.homes[i].isReachable ) {
            context_.tracer().debug( "Reached Home object '"+regData.homes[i].proxy->ice_toString()+"'" );
            ++aliveCount;
        }
        else {
            try
            {
                context_.tracer().info( "Removing Home object '"+regData.homes[i].proxy->ice_toString()+"'" );
                adminPrx->removeObject( regData.homes[i].proxy->ice_getIdentity() );
                ++deadCount;
            }
            catch ( const Ice::ObjectNotExistException& e )
            {
                stringstream ss; ss<<"(while removing object) Failed to talk to the registry: "<< e;
                context_.tracer().error( ss.str() );
                ++failedCount;
            }
            catch ( const IceGrid::ObjectNotRegisteredException& e )
            {
                stringstream ss; ss<<"(while removing object) Failed: "<< e;
                context_.tracer().error( ss.str() );
                ++failedCount;
            }
            catch ( const IceGrid::DeploymentException& e )
            {
                stringstream ss; ss<<"(while removing object) Failed: "<< e;
                context_.tracer().error( ss.str() );
                ++failedCount;
            }            
            catch ( const std::exception& e )
            {
                stringstream ss; ss<<"(while removing object) Failed: "<< e.what();
                context_.tracer().error( ss.str() );
                ++failedCount;
            }
        }
    }
    
    cout<<"registered  : "<<regData.homes.size()<<endl;
    cout<<"alive       : "<<aliveCount<<endl;
    cout<<"removed     : "<<deadCount<<endl;
}


void
MainThread::purgeAdapters( const IceGrid::AdminPrx& adminPrx )
{
    Ice::CommunicatorPtr ic = context_.communicator();

    //
    // get adapter adaptList
    //
    Ice::StringSeq adaptList = adminPrx->getAllAdapterIds();
    cout<<"retrieved list of "<<adaptList.size()<<" adapters"<<endl;

    orca::FQComponentName compName;
    Ice::Identity adminIdentity;

    int aliveCount = 0;
    int deadCount = 0;
        
    for ( unsigned int i=0; i<adaptList.size(); ++i ) 
    {
        compName = orcaice::toComponentName( adaptList[i] );
        adminIdentity = orcaice::toAdminIdentity( compName );
        
        // ping each component's Home interface
        const std::string facetName = orcaice::toAdminFacet( compName, "::orca::Home" );

        if ( orcacm::pingAdminObject( context_, ic->identityToString(adminIdentity), facetName ) ) 
        {
            cout<<adaptList[i]<<"\talive"<<endl;
            ++aliveCount;
        }
        // make sure it's an Orca component, it's platform compName should be non-empty
        else if ( !compName.platform.empty() ) 
        {
            try
            {
                cout<<adaptList[i]<<"\tfailed to ping, about to remove"<<endl;
                adminPrx->removeAdapter( adaptList[i] );
            }
            catch ( const IceGrid::AdapterNotExistException & e )
            {
                cout<<e<<endl;
            }
            catch ( const IceGrid::DeploymentException & e )
            {
                cout<<e<<endl;
            }
            
            ++deadCount;
        }
        else {
            cout<<"will not clean "<<compName.component<<endl;
        }
    }
    
    cout<<"registered  : "<<adaptList.size()<<endl;
    cout<<"alive       : "<<aliveCount<<endl;
    cout<<"removed     : "<<deadCount<<endl;
    cout<<"non-Orca    : "<<adaptList.size()-aliveCount-deadCount<<endl;
}
