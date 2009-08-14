/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <gbxutilacfr/exceptions.h>
#include "localstatus.h"
#include <iostream>

using namespace std;
using namespace hydroiceutil;

namespace {

void reportUnknownSubsystem( gbxutilacfr::Tracer& tracer, const std::string& subsysName, const std::string& activity="" )
{        
    stringstream ss;
    if ( !activity.empty() )
        ss << "(while " + activity + ") ";

    ss << "subsystem '"<<subsysName<<"' does not exist.";
    tracer.error( ss.str() );
    
    throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
}

std::string toString( const LocalSubsystemStatus& s )
{
    stringstream ss;
    ss << "state="<<gbxutilacfr::toString(s.state)
       <<" health="<<gbxutilacfr::toString(s.health)
       <<" msg='"<<s.message<<"'"
       <<" stall="<<(int)s.isStalled
       <<" interval="<<s.maxHeartbeatInterval;
    return ss.str();
}

std::string toString( const LocalComponentStatus& s )
{
    stringstream ss;
    ss << "LocalComponentStatus:" << endl
       << "infra="<<gbxutilacfr::toString(s.infrastructure)
       <<" pubInterval="<<s.publishIntervalSec;

    for ( LocalSubsystemStatusMap::const_iterator it = s.subsystems.begin(); it!=s.subsystems.end(); ++it )
        ss << endl << "  " << it->first << "\t" << toString( it->second );

    return ss.str();
}

}

/////////////////

LocalStatus::LocalStatus( gbxutilacfr::Tracer& tracer, const hydroutil::Properties& props )
    : tracer_(tracer)
{
    cout<<"LocalStatus: initializing with properties:"<<endl
        <<props.toString()<<endl;

    // alexm: maybe shouldn't be setting defaults at such low level?
    componentStatus_.publishIntervalSec = props.getPropertyAsDoubleWithDefault( "PublishPeriod", 30 );
    toleranceMultiplicative_ = props.getPropertyAsDoubleWithDefault( "StallTolerance.Multiplicative", 1.5 );
    toleranceAdditive_ = props.getPropertyAsDoubleWithDefault( "StallTolerance.Additive", 0.1 );

    cout<<"LocalStatus: loaded props: multi="<<toleranceMultiplicative_<<" add="<<toleranceAdditive_<<endl;
}

void 
LocalStatus::addSubsystem( const std::string& subsysName, double maxHeartbeatIntervalSec, gbxutilacfr::SubsystemType type )
{
    IceUtil::Mutex::Lock lock(localStatusMutex_);

    LocalSubsystemStatusMap::iterator it;
    it = componentStatus_.subsystems.find( subsysName );

    if ( it == componentStatus_.subsystems.end() )
    {
        stringstream ss;
        ss << "LocalStatus::"<<__func__<<"(): Adding new subsystem: '"<<subsysName<<"'";
        // this is a generic trace, not specific to the new subsystem
        tracer_.debug( ss.str() );

        // the timer is initialised to current time
        LocalSubsystemStatus newStatus;
        newStatus.type = type;
        newStatus.maxHeartbeatInterval = maxHeartbeatIntervalSec;

        componentStatus_.subsystems[subsysName] = newStatus;      

        // publish updated data (added subsystem)
        localPublish();  
    }
    else
    {
        stringstream ss;    
        ss << "(while adding subsystem) subsystem '"<<subsysName<<"' already exists.";
        // this is a generic trace, not specific to the new subsystem
        tracer_.error( ss.str() );
        
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }
}

void 
LocalStatus::removeSubsystem( const std::string& subsysName )
{
    IceUtil::Mutex::Lock lock(localStatusMutex_);

    LocalSubsystemStatusMap::iterator it = componentStatus_.subsystems.find( subsysName );
    if ( it == componentStatus_.subsystems.end() )
        reportUnknownSubsystem( tracer_, subsysName, "removing subsystem" );

    stringstream ss;
    ss << "LocalStatus::"<<__func__<<"(): Removing existing subsystem: '"<<subsysName<<"'";
    // this is a generic trace, not specific to the removed subsystem
    tracer_.debug( ss.str() );
    componentStatus_.subsystems.erase( it );

    // publish updated data (removed subsystem)
    localPublish();  
}

std::vector<std::string>
LocalStatus::subsystems()
{
    IceUtil::Mutex::Lock lock(localStatusMutex_);
    std::vector<std::string> names;
    for ( LocalSubsystemStatusMap::const_iterator it = componentStatus_.subsystems.begin(); it!=componentStatus_.subsystems.end(); ++it )
        names.push_back( it->first );

    return names;
}

gbxutilacfr::SubsystemStatus 
LocalStatus::subsystemStatus( const std::string& subsysName )
{
    IceUtil::Mutex::Lock lock(localStatusMutex_);

    LocalSubsystemStatusMap::iterator it = componentStatus_.subsystems.find( subsysName );
    if ( it == componentStatus_.subsystems.end() )
        reportUnknownSubsystem( tracer_, subsysName, "getting status" );
    
    LocalSubsystemStatus& sstat = it->second;
    return gbxutilacfr::SubsystemStatus( sstat.state, sstat.health, sstat.message, sstat.isStalled,
            sstat.heartbeatTimer.elapsedSec() / sstat.maxHeartbeatInterval );
}

gbxutilacfr::SubsystemState 
LocalStatus::infrastructureState()
{
    IceUtil::Mutex::Lock lock(localStatusMutex_);

    return componentStatus_.infrastructure;
}

void 
LocalStatus::setMaxHeartbeatInterval( const std::string& subsysName, double maxHeartbeatIntervalSec )
{
    IceUtil::Mutex::Lock lock(localStatusMutex_);

    LocalSubsystemStatusMap::iterator it = componentStatus_.subsystems.find( subsysName );
    if ( it == componentStatus_.subsystems.end() )
        reportUnknownSubsystem( tracer_, subsysName, "setting maximum heartbeat interval" );

    // Modifying previously-registered subsytem
    LocalSubsystemStatus &status = it->second;
    status.maxHeartbeatInterval = maxHeartbeatIntervalSec;
    status.heartbeatTimer.restart();
}

void 
LocalStatus::setSubsystemType( const std::string& subsysName, gbxutilacfr::SubsystemType type )
{
    IceUtil::Mutex::Lock lock(localStatusMutex_);

    LocalSubsystemStatusMap::iterator it = componentStatus_.subsystems.find( subsysName );
    if ( it == componentStatus_.subsystems.end() )
        reportUnknownSubsystem( tracer_, subsysName, "setting subsystem type" );

    // Modifying previously-registered subsytem
    LocalSubsystemStatus &status = it->second;
    status.type = type;
}

void 
LocalStatus::setSubsystemStatus( const std::string& subsysName, gbxutilacfr::SubsystemState state, 
        gbxutilacfr::SubsystemHealth health, const std::string& msg )
{
    internalSetStatus( subsysName, state, health, msg );
}

void 
LocalStatus::initialising( const std::string& subsysName, const std::string& msg )
{
    internalSetStatus( subsysName, gbxutilacfr::SubsystemInitialising, (gbxutilacfr::SubsystemHealth)-1, msg );
}

void 
LocalStatus::working( const std::string& subsysName, const std::string& msg )
{
    internalSetStatus( subsysName, gbxutilacfr::SubsystemWorking, (gbxutilacfr::SubsystemHealth)-1, msg );
}

void 
LocalStatus::finalising( const std::string& subsysName, const std::string& msg )
{
    internalSetStatus( subsysName, gbxutilacfr::SubsystemFinalising, (gbxutilacfr::SubsystemHealth)-1, msg );
}

void 
LocalStatus::ok( const std::string& subsysName, const std::string& msg )
{
    internalSetStatus( subsysName, (gbxutilacfr::SubsystemState)-1, gbxutilacfr::SubsystemOk, msg );
}

void 
LocalStatus::warning( const std::string& subsysName, const std::string& msg )
{
    assert( !msg.empty() && "Debug requires a message" );
    internalSetStatus( subsysName, (gbxutilacfr::SubsystemState)-1, gbxutilacfr::SubsystemWarning, msg );
}

void 
LocalStatus::fault( const std::string& subsysName, const std::string& msg )
{
    assert( !msg.empty() && "Debug requires a message" );
    internalSetStatus( subsysName, (gbxutilacfr::SubsystemState)-1, gbxutilacfr::SubsystemFault, msg );
}
    
void 
LocalStatus::heartbeat( const std::string& subsysName )
{
    // NOTE: we pass a empty message, but it will not be taken seriously because this is a heartbeat.
    string dummyMessage;
    internalSetStatus( subsysName, (gbxutilacfr::SubsystemState)-1, (gbxutilacfr::SubsystemHealth)-1, dummyMessage );
}
    
void 
LocalStatus::message( const std::string& subsysName, const std::string& msg )
{
    internalSetStatus( subsysName, (gbxutilacfr::SubsystemState)-1, (gbxutilacfr::SubsystemHealth)-1, msg );
}

void 
LocalStatus::internalSetStatus( const std::string& subsystemName, gbxutilacfr::SubsystemState state, 
                gbxutilacfr::SubsystemHealth health, const std::string& msg )
{
    assert( state!=gbxutilacfr::SubsystemIdle && "Idle state should not be reported from within the subsystem" );

    IceUtil::Mutex::Lock lock(localStatusMutex_);

    LocalSubsystemStatusMap::iterator it = componentStatus_.subsystems.find( subsystemName );
    if ( it == componentStatus_.subsystems.end() )
        reportUnknownSubsystem( tracer_, subsystemName, "setting subsystem status" );

    LocalSubsystemStatus& status = it->second;

    // we need a good reason to publish data.
    bool publishStatus = false;

    // restart the timer, regardless
    status.heartbeatTimer.restart();

    // if currently considered stalled, clear the 'stalled' flag.
    if ( status.isStalled ) {
        status.isStalled = false;
        tracer_.info( subsystemName, "Subsystem "+subsystemName+" recovered from a stall." );
        publishStatus = true;
    }

    // if this is a heartbeat, do nothing else
    // exception: if this heartbeat has just removed the stall condition.
    if ( state<0 && health<0 && !publishStatus )
        return;

    // state did not change
    if ( state<0 )
        state = status.state;

    // health did not change
    if ( health<0 )
        health = status.health;

    bool traceState = false;
    if ( state != status.state ) 
        traceState = true;
    
    bool traceHealth = true;
    // one case when NOT to trace: was OK, is Ok, no change in message
    if ( status.health == gbxutilacfr::SubsystemOk && health == gbxutilacfr::SubsystemOk && msg == status.message ) 
        traceHealth = false;

    if ( state != status.state || health != status.health || msg != status.message ) 
        publishStatus = true;

//     cout<<"DEBUG: traceState="<<(int)traceState<<" traceHealth="<<(int)traceHealth<<" publishStatus="<<(int)publishStatus<<endl;

    // apply changes
    status.state = state;
    status.health = health;
    status.message = msg;

    // give tracer feedback on state
    if ( traceState )
        tracer_.info( subsystemName, "Subsystem "+subsystemName+" changed state to "+gbxutilacfr::toString(status.state) );

    // give tracer feedback on status
    if ( traceHealth )
    {
        string trace = "Subsystem "+subsystemName+" status="+gbxutilacfr::toString(status.state)+"/"+gbxutilacfr::toString(status.health);
        if ( !status.message.empty() )
            trace += ": "+status.message;
        switch ( status.health )
        {
        case gbxutilacfr::SubsystemOk :
            tracer_.info( subsystemName, trace );
            break;
        case gbxutilacfr::SubsystemWarning :
            tracer_.warning( subsystemName, trace );
            break;
        case gbxutilacfr::SubsystemFault :
            tracer_.error( subsystemName, trace );
            break;
        }
    }

    // publish updated status (changed status)
    if ( publishStatus )
        localPublish();
}

void 
LocalStatus::infrastructureInitialising()
{
    IceUtil::Mutex::Lock lock(localStatusMutex_);
    
    if ( componentStatus_.infrastructure == gbxutilacfr::SubsystemInitialising )
        return;

    componentStatus_.infrastructure = gbxutilacfr::SubsystemInitialising;

    tracer_.info( "Component infrastructure changed state to Initialising." );

    localPublish();
}

void 
LocalStatus::infrastructureWorking()
{
    IceUtil::Mutex::Lock lock(localStatusMutex_);

    if ( componentStatus_.infrastructure == gbxutilacfr::SubsystemWorking )
        return;

    componentStatus_.infrastructure = gbxutilacfr::SubsystemWorking;

    tracer_.info( "Component infrastructure changed state to Working." );

    localPublish();
}

void 
LocalStatus::infrastructureFinalising()
{
    IceUtil::Mutex::Lock lock(localStatusMutex_);

    if ( componentStatus_.infrastructure == gbxutilacfr::SubsystemFinalising )
        return;

    componentStatus_.infrastructure = gbxutilacfr::SubsystemFinalising;

    tracer_.info( "Component infrastructure changed state to Finalising." );

    localPublish();
}

void 
LocalStatus::process()
{
    IceUtil::Mutex::Lock lock(localStatusMutex_);

    // No subsystems have been defined -- no status to report.
    if ( componentStatus_.subsystems.size() == 0 )
        return;

    // heartbeat check
    bool isJustStalled = false;
    for ( LocalSubsystemStatusMap::iterator it=componentStatus_.subsystems.begin(); it!=componentStatus_.subsystems.end(); ++it ) 
    {
        const std::string& subsysName = it->first;
        LocalSubsystemStatus& status = it->second;

        if ( (status.maxHeartbeatInterval > 0.0) && 
             (status.state != gbxutilacfr::SubsystemIdle ) &&
             (status.state != gbxutilacfr::SubsystemShutdown ) &&
             !status.isStalled &&
             (status.heartbeatTimer.elapsedSec() > 
                    (status.maxHeartbeatInterval * toleranceMultiplicative_ + toleranceAdditive_) ) )
        {
            // we just realized that this subsystem is stalled ...
            // NOTE: do NOT call internalSetStatus() because it will reset the heartbeat timer!
            status.isStalled = true;

            string trace = "Subsystem "+subsysName+" stalled with status="+gbxutilacfr::toString(status.state)+"/"+gbxutilacfr::toString(status.health);
            if ( !status.message.empty() )
                trace += ": "+status.message;
            tracer_.error( it->first, trace );

            // debug
//             stringstream ss;
//             ss << "Stall detected after " << status.heartbeatTimer.elapsedSec() << "sec elapsed";
//             tracer_.debug( ss.str() );

            isJustStalled = true;
        }
    }

    bool isPublishTime = ( publishTimer_.elapsedSec() >= componentStatus_.publishIntervalSec );

    // publish updated status (stalled or remote heartbeat)
    if ( isJustStalled || isPublishTime ) 
        localPublish();
}

// NOTE: this functions is called from several places, all protected by mutex
void
LocalStatus::localPublish()
{
//     tracer_.debug( "LocalStatus::localPublish(): "+toString( componentStatus_ ), 6 );

    // let derived classes publish status
    publishEvent( componentStatus_ );

    publishTimer_.restart();
}

void
LocalStatus::publishEvent( const LocalComponentStatus& componentStatus )
{
//     cout<<"LocalStatus::publish()"<<endl;
    // POSSIBLY TODO: dump to tracer
}
