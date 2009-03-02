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
#include <orcaice/orcaice.h>

#include "mainthread.h"

using namespace std;

namespace systemstatus {
    
namespace {
    
// one way to define the system: by specifying all components upfront in the config file
vector<orca::FQComponentName> getSystemComponentsFromConfig( const orcaice::Context& context )
{   
    Ice::PropertiesPtr props = context.properties();
    std::string prefix = context.tag()+".Config.";
    
    vector<string> componentList;
    char delimiter = ' ';
    int ret = orcaice::getPropertyAsStringVector( props, prefix+"ComponentList", componentList, delimiter );
    if (ret!=0)
        throw gbxutilacfr::Exception( ERROR_INFO, "Problem reading ComponentList from config file" );
    
    vector<orca::FQComponentName> fqCompNames;
    for ( unsigned int i=0; i<componentList.size(); ++i )
    {
        orca::FQComponentName fqCompName = orcaice::toComponentName( componentList[i] );
        if ( fqCompName.platform.empty() && fqCompName.component.empty() ) {
            stringstream ss;
            ss << "Failed to parse component name " << i << " : '" << componentList[i] << "'";
            throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
        }

        fqCompNames.push_back( fqCompName );
    }
    
    return fqCompNames;
}
    
//
// converts from internal multimap to Slice-defined representation
//
void convert( const multimap<string,orca::ObservedComponentStatus> &from, 
              orca::SystemStatusData                               &to,
              int                                                   sleepTimeSec )
{    
    orcaice::setToNow( to.timeStamp );
    to.publishIntervalSec = sleepTimeSec*5;
    
    multimap<string,orca::ObservedComponentStatus>::const_iterator it;
    pair<multimap<string,orca::ObservedComponentStatus>::const_iterator,multimap<string,orca::ObservedComponentStatus>::const_iterator> ret;
    
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
        vector<orca::ObservedComponentStatus> componentsPerPlatform;
        
        // obtain all records of a given platform name
        ret = from.equal_range( uniquePlatformNames[i] );

        // create a vector of ObservedComponentStatus
        for (it=ret.first; it!=ret.second; ++it)
        {
            componentsPerPlatform.push_back( (*it).second );
        }
        
        to.systemStatus[uniquePlatformNames[i]] = componentsPerPlatform;
    }
}

} // end of namespace
    

MainThread::MainThread( const orcaice::Context & context ) :
    orcaice::SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    context_(context)
{
}

void 
MainThread::initialise()
{
    // multi-try
    orcaice::activate( context_, this, subsysName() );
    // check for stop signal after retuning from multi-try
    if ( isStopping() )
        return;
     
    // provided interface
    systemStatusIface_ = new orcaifaceimpl::SystemStatusImpl( "SystemStatus", context_ );
    systemStatusIface_->initInterface( this );

    Ice::PropertiesPtr props = context_.properties();
    std::string prefix = context_.tag()+".Config.";
    
    // init the job queue
    hydroiceutil::JobQueue::Config config;
    config.threadPoolSize = orcaice::getPropertyAsIntWithDefault( props, prefix+"JobQueueThreadPoolSize", 10 );
    config.queueSizeWarn = orcaice::getPropertyAsIntWithDefault( props, prefix+"JobQueueSizeWarning", 2 );
    config.traceAddEvents = false;
    config.traceStartEvents = false;
    config.traceDoneEvents = false;
    
    jobQueue_ = new hydroiceutil::JobQueue( context_.tracer(), config );
    
    // create component monitors
    vector<orca::FQComponentName> fqCompNames; 

    fqCompNames = getSystemComponentsFromConfig( context_ );
    for ( unsigned int i=0; i<fqCompNames.size(); ++i )
    {
        ComponentMonitor mon( fqCompNames[i], jobQueue_, context_ );
        monitors_.push_back(mon);
    }
}

void
MainThread::work()
{     
    orca::SystemStatusData data;
    multimap<string,orca::ObservedComponentStatus> obsCompStateMultiMap;
    const int sleepTimeSec = 2;
    
    while ( !isStopping() )
    {
        context_.tracer().info( "MainThread: waiting..." );
        
        // get status data from all monitors
        obsCompStateMultiMap.clear();
        for (unsigned int i=0; i<monitors_.size(); i++)
        {
            orca::ObservedComponentStatus obsCompStat;
            string platformName;
            monitors_[i].getComponentStatus( obsCompStat );
            obsCompStateMultiMap.insert(pair<string,orca::ObservedComponentStatus>(obsCompStat.name.platform,obsCompStat));
        }
        
        // convert and tell the world
        convert( obsCompStateMultiMap, data, sleepTimeSec );
        systemStatusIface_->localSetAndSend( data );
                
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(sleepTimeSec));
        
    } // end of main loop
}

}



