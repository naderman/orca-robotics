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
#include <orcaicegrid/icegridmanager.h>

#include "mainthread.h"

using namespace std;
using namespace registryclean;

namespace {

void purgeObjects( orcaicegrid::AbstractIceGridManager* igManager, 
                   const orcaice::Context& context, gbxutilacfr::Stoppable* activity )
{
    std::string locatorString = context.communicator()->getDefaultLocator()->ice_toString();

    //
    // Remote call to Registry (don't need Admin interface for this)
    //
    context.tracer().info( "Getting information about Home objects from the Registry." );
    orcacm::RegistryHomeData regData;
    regData = orcacm::getRegistryHomeData( context, locatorString );

    if ( !regData.isReachable ) {
        context.tracer().error( "Registry '"+locatorString+"' is unreachable" );
        // nothing else we can do
        context.shutdown();
    }

    stringstream ss;
    ss<<"Retrieved list of "<<regData.homes.size()<<" home objects";
    context.tracer().info( ss.str() );

    //
    // Remote calls to individual home objects
    // TODO: use thread pool!
    //
    orcacm::pingHomeObjects( regData, context, activity );

    int aliveCount = 0;
    int deadCount = 0;
    int reservedCount = 0;
    int missingCount = 0;
    int failedCount = 0;
        
    context.tracer().info( "Starting to purge Home objects." );
    for ( unsigned int i=0; i<regData.homes.size(); ++i ) 
    {    
        // ping each component's Home interface
        if ( regData.homes[i].isReachable ) {
//             context.tracer().debug( "Reached Home object '"+regData.homes[i].proxy->ice_toString()+"'" );
            ++aliveCount;
        }
        else {
            try
            {
//                 context.tracer().info( "Removing Home object '"+regData.homes[i].proxy->ice_toString()+"'" );
                igManager->removeObject( regData.homes[i].proxy->ice_getIdentity() );
                ++deadCount;
            }
            catch ( const Ice::ObjectNotExistException& e )
            {
                cout<<"Object has gone missing since last check: "<<e<<endl;
                ++missingCount;
            }
            catch ( const IceGrid::ObjectNotRegisteredException& e )
            {
                cout<<"Object has gone missing since last check: "<<e<<endl;
                ++missingCount;
            }
            catch ( const IceGrid::DeploymentException& e )
            {
                // this is expected when the adapter is down but the registry keeps a reservation for it
                // because it's in the application.
                ++reservedCount;
            }            
            catch ( const std::exception& e )
            {
                stringstream ss; ss<<"(while removing object) Failed: "<< e.what();
                context.tracer().error( ss.str() );
                ++failedCount;
            }
        }
    }
    
    cout<<"registered  : "<<regData.homes.size()<<endl;
    cout<<"alive       : "<<aliveCount<<endl;
    cout<<"removed     : "<<deadCount<<endl;
    cout<<"reserved    : "<<reservedCount<<endl;
    cout<<"missing     : "<<missingCount<<endl;
    cout<<"failed      : "<<failedCount<<endl;
}


void purgeAdapters( orcaicegrid::AbstractIceGridManager* igManager, 
                    const orcaice::Context& context )
{
    Ice::CommunicatorPtr ic = context.communicator();

    //
    // get adapter adaptList
    //
    // TODO: catch exceptions
    Ice::StringSeq adaptList = igManager->getAllAdapterIds();
    cout<<"retrieved list of "<<adaptList.size()<<" adapters"<<endl;

    orca::FQComponentName compName;
    Ice::Identity adminIdentity;

    int aliveCount = 0;
    int deadCount = 0;
    int reservedCount = 0;
    int missingCount = 0;
    int failedCount = 0;
        
    for ( unsigned int i=0; i<adaptList.size(); ++i ) 
    {
        compName = orcaice::toComponentName( adaptList[i] );
        adminIdentity = orcaice::toAdminIdentity( compName );
        
        // ping each component's Home interface
        const std::string facetName = orcaice::toAdminFacet( compName, "::orca::Home" );

        if ( orcacm::pingAdminObject( context, ic->identityToString(adminIdentity), facetName ) ) 
        {
            cout<<adaptList[i]<<"\talive"<<endl;
            ++aliveCount;
        }
        // make sure it's an Orca component, it's platform compName should be non-empty
        else if ( !compName.platform.empty() ) 
        {
            try
            {
//                 cout<<adaptList[i]<<"\tfailed to ping, about to remove"<<endl;
                igManager->removeAdapter( adaptList[i] );
                ++deadCount;
            }
            catch ( const IceGrid::AdapterNotExistException & e )
            {
                cout<<"Adapter has gone missing since last check: "<<e<<endl;
                ++missingCount;
            }
            catch ( const IceGrid::DeploymentException & e )
            {
                // this is expected when the adapter is down but the registry keeps a reservation for it
                // because it's in the application.
                ++reservedCount;
            }   
            catch ( const std::exception& e )
            {
                stringstream ss; ss<<"(while removing adapter) Failed: "<< e.what();
                context.tracer().error( ss.str() );
                ++failedCount;
            }
            
        }
        else {
            cout<<"will not clean "<<compName.component<<endl;
        }
    }
    
    cout<<"registered  : "<<adaptList.size()<<endl;
    cout<<"alive       : "<<aliveCount<<endl;
    cout<<"removed     : "<<deadCount<<endl;
    cout<<"reserved    : "<<reservedCount<<endl;
    cout<<"missing     : "<<missingCount<<endl;
    cout<<"failed      : "<<failedCount<<endl;
    cout<<"non-Orca    : "<<adaptList.size()-aliveCount-deadCount-reservedCount-missingCount-failedCount<<endl;
}

}

/////////////////////////////////

MainThread::MainThread( const orcaice::Context &context ) : 
    orcaice::SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    context_(context)
{
}

void
MainThread::work()
{
    orcaicegrid::IceGridManagerPtr iceGridManager_ = new orcaicegrid::IceGridManager( context_ );
    iceGridManager_->start();
    context_.tracer().debug( "Started IceGrid manager thread", 3 );

    //
    // Well-known objects
    //
    context_.tracer().info( "Purging well-known objects." );
    purgeObjects( iceGridManager_.get(), context_, this );

    //
    // Adapters
    //
    context_.tracer().info( "Purging adapters." );
    purgeAdapters( iceGridManager_.get(), context_ );

    // we are done
    context_.shutdown();
}
