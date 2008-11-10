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
#include <orcaifacestring/status.h>
#include "componentstatusaggregator.h"

using namespace std;

namespace orcaice {
namespace detail {

namespace {
    
void 
convert( const hydroiceutil::NameStatusMap &from, orca::SubsystemStatusDict &to )
{
    hydroiceutil::NameStatusMap::const_iterator it;
    
    for ( it=from.begin(); it!=from.end(); ++it ) 
    {
        switch ( it->second.state ) 
        {
            case gbxutilacfr::SubsystemIdle :
                to[it->first].state = orca::SubsystemIdle;
                break;
            case gbxutilacfr::SubsystemInitialising :
                to[it->first].state = orca::SubsystemInitialising;
                break;
            case gbxutilacfr::SubsystemWorking :
                to[it->first].state = orca::SubsystemWorking;
                break;
            case gbxutilacfr::SubsystemFinalising :
                to[it->first].state = orca::SubsystemFinalising;
                break;
            case gbxutilacfr::SubsystemShutdown :
                to[it->first].state = orca::SubsystemShutdown;
                break;
        }
        switch ( it->second.health ) 
        {
            case gbxutilacfr::SubsystemOk :
                to[it->first].health = orca::SubsystemOk;
                break;
            case gbxutilacfr::SubsystemWarning :
                to[it->first].health = orca::SubsystemWarning;
                break;
            case gbxutilacfr::SubsystemFault :
                to[it->first].health = orca::SubsystemFault;
                break;
            case gbxutilacfr::SubsystemStalled :
                to[it->first].health = orca::SubsystemStalled;
                break;
        }
        to[it->first].message = it->second.message;
        to[it->first].sinceHeartbeat = 
                (Ice::Float)(it->second.heartbeatTimer.elapsedSec() / it->second.maxHeartbeatInterval);
    }
}

orca::ComponentState 
subStateToCompState ( const orca::SubsystemState &subsystemState)
{
    switch (subsystemState)
    {
        case orca::SubsystemIdle: 
            return orca::CompInactive;
        case orca::SubsystemInitialising: 
            return orca::CompInitialising;
        case orca::SubsystemWorking: 
            return orca::CompActive;
        case orca::SubsystemFinalising: 
            return orca::CompFinalising;
        case orca::SubsystemShutdown: 
            return orca::CompFinalising;   
        default: {
            cout<<"########## SubsystemState="<<subsystemState<<endl;
            assert( false && "Unknown SubsystemState" );     
        }
    } 
}

orca::ComponentHealth
subHealthToCompHealth( const orca::SubsystemHealth &subsystemHealth )
{
    switch (subsystemHealth)
    {
        case orca::SubsystemOk:
            return orca::CompOk;
        case orca::SubsystemWarning: 
            return orca::CompWarning;
        case orca::SubsystemFault:
            return orca::CompFault;
        case orca::SubsystemStalled:
            return orca::CompStalled;
        default: {
            cout<<"########## SubsystemHealth="<<subsystemHealth<<endl;
            assert( false && "Unknown SubsystemHealth");
        }
    }
}   
    
void convertWorstHealth( const hydroiceutil::LocalComponentStatus &from, 
                         const orca::FQComponentName              &name, 
                         orca::ComponentStatus                    &to )
{
    // initialisation, will be overwritten
    to.state = orca::CompInactive;
    to.health = orca::CompOk;
    to.timeUp = 0;
    
    // conversions: name and subsystems
    to.name = name;
    convert( from.subsystems, to.subsystems );
    
    // if no subsystems exist, we're done
    if (to.subsystems.size()==0) return;
    
    //
    // ComponentState and ComponentHealth conversions
    //
    const orca::SubsystemStatusDict &subSysSt = to.subsystems;
    map<string,orca::SubsystemStatus>::const_iterator itWorstHealth;
    orca::SubsystemHealth worstHealth = orca::SubsystemOk;           
    
    for (map<string,orca::SubsystemStatus>::const_iterator it=subSysSt.begin(); it!=subSysSt.end(); ++it)
    {
        // the >= guarantees that itWorstHealth is set at least once
        if (it->second.health >= worstHealth ) {
            worstHealth = it->second.health;
            itWorstHealth = it;
        }
    }
    
    to.health = subHealthToCompHealth( worstHealth );
    to.state = subStateToCompState(itWorstHealth->second.state);    
}

orca::ComponentState subStateToCompState( const gbxutilacfr::SubsystemState &subsystemState )
{    
    switch (subsystemState)
    {
        case gbxutilacfr::SubsystemIdle: 
            return orca::CompInactive;
        case gbxutilacfr::SubsystemInitialising: 
            return orca::CompInitialising;
        case gbxutilacfr::SubsystemWorking: 
            return orca::CompActive;
        case gbxutilacfr::SubsystemFinalising: 
            return orca::CompFinalising;
        case gbxutilacfr::SubsystemShutdown: 
            return orca::CompFinalising;   
        default: {
            cout<<"########## SubsystemState="<<subsystemState<<endl;
            assert( false && "Unknown SubsystemState" );    
        }
    } 
}

orca::ComponentHealth
subHealthToCompHealth( const gbxutilacfr::SubsystemHealth &subsystemHealth )
{
    switch (subsystemHealth)
    {
        case gbxutilacfr::SubsystemOk:
            return orca::CompOk;
        case gbxutilacfr::SubsystemWarning: 
            return orca::CompWarning;
        case gbxutilacfr::SubsystemFault:
            return orca::CompFault;
        case gbxutilacfr::SubsystemStalled:
            return orca::CompStalled;
        default:
            assert( false && "Unknown SubsystemHealth");
    }
}  

orca::ComponentState extractCompState( const hydroiceutil::LocalComponentStatus &from )
{    
    bool anybodyIdle = false;
    bool anybodyInitialising = false;
    bool anybodyFinalising = false;
    bool anybodyShutdown = false;
    bool everybodyWorking = true;
    
    hydroiceutil::NameStatusMap::const_iterator it;
    
    for ( it=from.subsystems.begin(); it!=from.subsystems.end(); ++it ) 
    {
        switch ( it->second.state ) 
        {
            case gbxutilacfr::SubsystemIdle:
                anybodyIdle = true;
//                 cout << "========== IDLE ======== " << endl;
                break;
            case gbxutilacfr::SubsystemInitialising:
                anybodyInitialising = true;
//                 cout << "========== INIT ======== " << endl;
                break;
            case gbxutilacfr::SubsystemWorking:
//                 cout << "========== WORKING ======== " << endl;
                // do nothing
                break;
            case gbxutilacfr::SubsystemFinalising:
                if (it->second.type != gbxutilacfr::SubsystemEarlyExit) {
                    anybodyFinalising = true;
//                     cout << "========== FINAL ======== " << endl;
                }
                break;
            case gbxutilacfr::SubsystemShutdown:
                anybodyShutdown = true;
//                 cout << "========== SHUT ======== " << endl;
                break;
        }
        if (anybodyIdle || anybodyInitialising || anybodyFinalising || anybodyShutdown ) {
            everybodyWorking = false;
            break;
        }
    }
    
    orca::ComponentState state;
    
    if ( everybodyWorking ) {
        return orca::CompActive;
    } else {
        if (anybodyIdle)
            state = orca::CompInactive;
        if (anybodyInitialising)
            state = orca::CompInitialising;
        if (anybodyFinalising)
            state = orca::CompFinalising;
        if (anybodyShutdown)
            state = orca::CompFinalising;
    }
    
    return state;
    
}

void convertStateBased( const hydroiceutil::LocalComponentStatus &from, 
                        const orca::FQComponentName              &name, 
                        orca::ComponentStatus                    &to )
{
    // initialisation
    to.timeUp = 0;
    
    // conversions: name and subsystems
    to.name = name;
    to.publishIntervalSec = from.publishIntervalSec;
    
    // if no subsystems exist, the state is the infrastructure state, health is ok, and we return
    if (from.subsystems.size()==0) {
        to.state = subStateToCompState( from.infrastructure );
        to.health = orca::CompOk;
//         cout << "============== No subsystems. Will use infrastructure state " << to.health << endl;
        return;
    }
    
    // convert subsystems
    convert( from.subsystems, to.subsystems );
    
    //
    // ComponentState
    //
    if (from.infrastructure != gbxutilacfr::SubsystemWorking)
        to.state = subStateToCompState( from.infrastructure );
    else
        to.state = extractCompState( from );
    
    //
    // ComponentHealth (independent of ComponentState)
    //
    gbxutilacfr::SubsystemHealth worstHealth = gbxutilacfr::SubsystemOk;    
    hydroiceutil::NameStatusMap::const_iterator it;
    
    for ( it=from.subsystems.begin(); it!=from.subsystems.end(); ++it ) 
    {
        // ">=" guarantees that worstHealth is set at least once
        if (it->second.health >= worstHealth )
            worstHealth = it->second.health;
    }
    to.health = subHealthToCompHealth( worstHealth );
}

    
} // end of namespace
    
ComponentStatusAggregator::ComponentStatusAggregator( AggregatorPolicy policy ) :
        policy_(policy)
{
}

void 
ComponentStatusAggregator::convert( const hydroiceutil::LocalComponentStatus &from,
                                    const orca::FQComponentName              &name, 
                                    orca::ComponentStatus                    &to )
{
    switch (policy_)
    {
        case AggregatorPolicyWorstHealth:
            convertWorstHealth( from, name, to );
            break;
        case AggregatorPolicyStateBased:
            convertStateBased( from, name, to );
            break;
        default:
            assert( false && "unkown policy" );    
    }
    
}

}
}
