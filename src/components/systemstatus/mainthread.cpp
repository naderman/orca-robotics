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
#include <hydroiceutil/jobqueue.h>
#include <orcaice/orcaice.h>

#include "mainthread.h"

using namespace std;

namespace systemstatus {
    
// converts from internal to Slice-defined representation
void convert( const multimap<string,StatusDetails> &from, 
              orca::SystemStatusData               &to )
{    
    multimap<string,StatusDetails>::const_iterator it;
    pair<multimap<string,StatusDetails>::const_iterator,multimap<string,StatusDetails>::const_iterator> ret;
    
    // assemble a vector of *unique* platform names
    vector<string> uniquePlatformNames;
    for (it=from.begin(); it!=from.end(); ++it)
    {
        const string &platform = it->first;
        bool isNew = true;
        for (unsigned int i=0; i<uniquePlatformNames.size(); i++ )
        {
            if (uniquePlatformNames[i]==platform) {
                isNew = false;
                break;
            }
        }
        if (isNew) 
            uniquePlatformNames.push_back( platform );
    }
    
    // go through all unique platforms and create a map indexed by platform name
    for ( unsigned int i=0; i<uniquePlatformNames.size(); i++ )
    {
        vector<orca::ComponentStatusData> componentsPerPlatform;
        
        // obtain all records of a given platform name
        ret = from.equal_range( uniquePlatformNames[i] );
        
        // create a vector of ComponentStatusData
        for (it=ret.first; it!=ret.second; ++it)
        {
            orca::ComponentStatusData compStatData;
            compStatData.isDataStale = (*it).second.isDataStale;
            compStatData.data = (*it).second.data;
            componentsPerPlatform.push_back( compStatData );
        }
        
        to[uniquePlatformNames[i]] = componentsPerPlatform;
    }
}
    

MainThread::MainThread( const orcaice::Context & context ) :
    orcaice::SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    context_(context)
{
}

vector<PlatformComponentPair>
MainThread::getComponentPlatformPairs()
{   
    vector<PlatformComponentPair> pairs;
    
    // get all tags starting with "Status"
    vector<std::string> requiredTags = orcaice::getRequiredTags( context_, "Status" );
    
    for (unsigned int i=0; i<requiredTags.size(); i++)
    {
        string proxyString = orcaice::getRequiredInterfaceAsString( context_, requiredTags[i] );
        string resolvedProxyString = orcaice::resolveLocalPlatform( context_, proxyString );
        orca::FQInterfaceName name = orcaice::toInterfaceName( resolvedProxyString );
        
        PlatformComponentPair pair;
        pair.platformName = name.platform;
        pair.componentName = name.component;
        pairs.push_back(pair);
//         cout << requiredTags[i] << ": " << resolvedProxyString << ": " << name.platform << ": " << name.component << endl;
    }
    return pairs;
}

void 
MainThread::createMonitors()
{
    Ice::PropertiesPtr props = context_.properties();
    std::string prefix = context_.tag()+".Config.";
    
    hydroiceutil::JobQueue::Config config;
    config.threadPoolSize = orcaice::getPropertyAsIntWithDefault( props, prefix+"JobQueueThreadPoolSize", 10 );
    config.queueSizeWarn = orcaice::getPropertyAsIntWithDefault( props, prefix+"JobQueueSizeWarning", 2 );
    config.traceAddEvents = false;
    config.traceStartEvents = false;
    config.traceDoneEvents = false;
    
    jobQueue_ = new hydroiceutil::JobQueue( context_.tracer(), config );
    
    vector<PlatformComponentPair> pairs = getComponentPlatformPairs();
    for ( unsigned int i=0; i<pairs.size(); i++)
    {
        ComponentMonitor mon( jobQueue_, pairs[i].platformName, pairs[i].componentName, context_ );
        monitors_.push_back(mon);
    }
}

void
MainThread::walk()
{    
    subStatus().initialising();

    // multi-try
    orcaice::activate( context_, this, subsysName() );
     
    // provided interface
    systemStatusIface_ = new orcaifaceimpl::SystemStatusImpl( "SystemStatus", context_ );
    systemStatusIface_->initInterface( this );

    // create the monitors
    createMonitors();
    
    subStatus().working();

    //
    // Main loop
    //
    // slice-defined systemstatus
    orca::SystemStatusData data;
    
    // statusDetails referenced by platformName
    multimap<string,StatusDetails> systemStatusDetails;
    
    while ( !isStopping() )
    {
        context_.tracer().info( "MainThread: waiting..." );
        
        // get status data from all monitors
        systemStatusDetails.clear();
        for (unsigned int i=0; i<monitors_.size(); i++)
        {
            StatusDetails details;
            string platformName;
            monitors_[i].getComponentStatus( platformName, details );
            if (details.dataAvailable) 
                systemStatusDetails.insert(pair<string,StatusDetails>(platformName,details));
        }
        
        // convert and tell the world
        convert( systemStatusDetails, data );
        systemStatusIface_->localSetAndSend( data );
                
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
        
    } // end of main loop
}

}



