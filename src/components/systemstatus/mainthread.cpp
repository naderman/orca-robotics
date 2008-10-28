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
    
void convert( const map<string,StatusDetails> &from, 
              orca::SystemStatusData          &to )
{    
    for ( map<string,StatusDetails>::const_iterator it=from.begin(); it!=from.end(); ++it )
    {
        orca::ComponentStatusData compStatData;
        compStatData.isDataStale = it->second.isDataStale;
        compStatData.data = it->second.data;
        to[it->first] = compStatData;
    }
}
    

MainThread::MainThread( const orcaice::Context & context ) :
    SafeThread( context.tracer() ),
    context_(context)
{
}

MainThread::~MainThread()
{
}

std::map<string,string> 
MainThread::getComponentPlatformPairs()
{
    std::map<string,string> pairs;
    
    // get all tags starting with "Status"
    vector<std::string> requiredTags = orcaice::getRequiredTags( context_, "Status" );
    
    for (unsigned int i=0; i<requiredTags.size(); i++)
    {
        string proxyString = orcaice::getRequiredInterfaceAsString( context_, requiredTags[i] );
        string resolvedProxyString = orcaice::resolveLocalPlatform( context_, proxyString );
        orca::FQInterfaceName name = orcaice::toInterfaceName( resolvedProxyString );
        pairs[name.component] = name.platform;
        
        //cout << requiredTags[i] << ": " << resolvedProxyString << ": " << name.platform << ": " << name.component << endl;
    }
    return pairs;
}

void 
MainThread::createMonitors()
{
    Ice::PropertiesPtr props = context_.properties();
    std::string prefix = context_.tag()+".Config.";
    
    hydroiceutil::JobQueue::Config config;
    config.threadPoolSize = orcaice::getPropertyAsIntWithDefault( props, prefix+"JobQueueThreadPoolSize", 2 );
    config.queueSizeWarn = orcaice::getPropertyAsIntWithDefault( props, prefix+"JobQueueSizeWarning", 2 );
    config.traceAddEvents = orcaice::getPropertyAsIntWithDefault( props, prefix+"JobQueueTraceAdded", 0 );
    config.traceStartEvents = orcaice::getPropertyAsIntWithDefault( props, prefix+"JobQueueTraceStart", 0 );
    config.traceDoneEvents = orcaice::getPropertyAsIntWithDefault( props, prefix+"JobQueueTraceDone", 0 );
    
    jobQueue_ = new hydroiceutil::JobQueue( context_.tracer(), config );
    
    std::map<string,string> pairs = getComponentPlatformPairs();
    
    for ( map<string,string>::const_iterator it=pairs.begin(); it!=pairs.end(); ++it )
    {
        ComponentMonitor mon( jobQueue_, it->second, it->first, context_ );
        monitors_.push_back(mon);
    }
}

void
MainThread::walk()
{    
    // multi-try
    orcaice::activate( context_, this );
     
    // provided interface
    systemStatusIface_ = new orcaifaceimpl::SystemStatusImpl( "SystemStatus", context_ );
    systemStatusIface_->initInterface( this );

    // create the monitors
    createMonitors();
    
    
    //
    // Main loop
    //
    
    orca::SystemStatusData data;
    map<string,StatusDetails> systemStatusDetails;
    
    while ( !isStopping() )
    {
        context_.tracer().info( "MainThread: waiting..." );
        
        // get status data from all monitors
        systemStatusDetails.clear();
        for (unsigned int i=0; i<monitors_.size(); i++)
        {
            StatusDetails details;
            string platComp;
            monitors_[i].getComponentStatus( platComp, details ); 
            systemStatusDetails[platComp] = details;
        }
        
        // convert and tell the world
        convert( systemStatusDetails, data );
        systemStatusIface_->localSetAndSend( data );
                
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
        
    } // end of main loop

    context_.tracer().debug( "MainThread: stopped.",2 );
}

}



