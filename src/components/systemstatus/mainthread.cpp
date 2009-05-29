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

} // end of namespace
    

MainThread::MainThread( const orcaice::Context & context ) :
    orcaice::SubsystemThread( context.tracer(), context.status(), "MainThread" ),
    context_(context)
{
}

void 
MainThread::initialise()
{    
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

    // provided interface
    systemStatusIface_ = new orcaifaceimpl::SystemStatusImpl( "SystemStatus", context_ );
    systemStatusIface_->initInterface( this );
}

void
MainThread::work()
{     
    const int sleepTimeSec = 2;

//     multimap<string,orca::ObservedComponentStatus> obsCompStateMultiMap;

    orca::SystemStatusData data;
    data.publishIntervalSec = sleepTimeSec*5;

    orca::EstimatedComponentStatus estCompStatus;

    // NOTE: we don't try to pre-populate the data structure. this way
    // we can later implement a dynamic status tracker based on some kind of
    // component discovery.    

    // This thread polls all monitors periodically.
    // This polling period determines the responsiveness of stale-detection
    // and the action to resubscribe.
    // 
    // data transfer is inefficient: the entire system state is transmitted every time step.
    // could optimize by transmitting only the status which has changed.
    while ( !isStopping() )
    {        
        orcaice::setToNow( data.timeStamp );
        data.systemStatus.clear();

        // get status data from all monitors
        for ( unsigned int i=0; i<monitors_.size(); ++i ) 
        { 
            estCompStatus = monitors_[i].getComponentStatus();
    
            data.systemStatus[ estCompStatus.name.platform ].push_back( estCompStatus );
        }        
        // gave it the interface implementation
        systemStatusIface_->localSetAndSend( data );
                
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(sleepTimeSec));
        
    } // end of main loop
}

}



