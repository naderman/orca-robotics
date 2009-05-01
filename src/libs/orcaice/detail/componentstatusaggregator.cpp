/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed net you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include "componentstatusaggregator.h"

using namespace std;

namespace orcaice {
namespace detail {

namespace {
    
void 
convert( const hydroiceutil::LocalSubsystemStatusMap& local, orca::SubsystemStatusDict& net )
{
    hydroiceutil::LocalSubsystemStatusMap::const_iterator it;
    
    for ( it=local.begin(); it!=local.end(); ++it ) 
    {
        const hydroiceutil::LocalSubsystemStatus& locStatus = it->second;

        orca::SubsystemStatus& netStatus = net[it->first];

        switch ( locStatus.state ) 
        {
            case gbxutilacfr::SubsystemIdle :
                netStatus.state = orca::SubsystemIdle;
                break;
            case gbxutilacfr::SubsystemInitialising :
                netStatus.state = orca::SubsystemInitialising;
                break;
            case gbxutilacfr::SubsystemWorking :
                netStatus.state = orca::SubsystemWorking;
                break;
            case gbxutilacfr::SubsystemFinalising :
                netStatus.state = orca::SubsystemFinalising;
                break;
            case gbxutilacfr::SubsystemShutdown :
                netStatus.state = orca::SubsystemShutdown;
                break;
        }

        switch ( locStatus.health ) 
        {
            case gbxutilacfr::SubsystemOk :
                netStatus.health = orca::SubsystemOk;
                break;
            case gbxutilacfr::SubsystemWarning :
                netStatus.health = orca::SubsystemWarning;
                break;
            case gbxutilacfr::SubsystemFault :
                netStatus.health = orca::SubsystemFault;
                break;
        }

        netStatus.message = locStatus.message;

        netStatus.isStalled = locStatus.isStalled;

        netStatus.sinceHeartbeat = 
            (Ice::Float)(locStatus.heartbeatTimer.elapsedSec() / locStatus.maxHeartbeatInterval);
    }
}  

orca::ComponentState toComponentState( const gbxutilacfr::SubsystemState &subsystemState )
{    
    switch (subsystemState)
    {
        case gbxutilacfr::SubsystemIdle: 
            return orca::CompInitialising;
        case gbxutilacfr::SubsystemInitialising: 
            return orca::CompInitialising;
        case gbxutilacfr::SubsystemWorking: 
            return orca::CompWorking;
        case gbxutilacfr::SubsystemFinalising: 
            return orca::CompFinalising;
        case gbxutilacfr::SubsystemShutdown: 
            return orca::CompFinalising;   
        default:
            assert( !"Unknown subsystem state" );    
    } 
}

orca::ComponentHealth toComponentHealth( const gbxutilacfr::SubsystemHealth &subsystemHealth )
{
    switch (subsystemHealth)
    {
        case gbxutilacfr::SubsystemOk:
            return orca::CompOk;
        case gbxutilacfr::SubsystemWarning: 
            return orca::CompWarning;
        case gbxutilacfr::SubsystemFault:
            return orca::CompFault;
        default:
            assert( !"Unknown subsystem health");
    }
}  

/*
alexm: disabled because it's not currently used

void convertWorstHealth( const hydroiceutil::LocalComponentStatus &local, 
                         const orca::FQComponentName              &name, 
                         orca::ComponentStatus                    &net )
{
    // initialisation, will be overwritten
    net.state = orca::CompInitialising;
    net.health = orca::CompOk;
    net.timeUp = 0;
    
    // conversions: name and subsystems
    net.name = name;
    net.publishIntervalSec = local.publishIntervalSec;
    
    // if no subsystems exist, the state is the infrastructure state, health is ok, and we return
    if (local.subsystems.size()==0) {
        net.state = toComponentState( local.infrastructure );
        net.health = orca::CompOk;
        return;
    }
    
    // convert subsystems
    convert( local.subsystems, net.subsystems );
    
    //
    // ComponentState and ComponentHealth conversions
    //
    gbxutilacfr::SubsystemHealth worstHealth = gbxutilacfr::SubsystemOk;    
    hydroiceutil::LocalSubsystemStatusMap::const_iterator itWorstHealth;
    
    for ( hydroiceutil::LocalSubsystemStatusMap::const_iterator it=local.subsystems.begin(); it!=local.subsystems.end(); ++it ) 
    {
        // ">=" guarantees that itWorstHealth is set at least once
        if (it->second.health >= worstHealth )
            worstHealth = it->second.health;
            itWorstHealth = it;
    }
    net.health = toComponentHealth( worstHealth );
    net.state = toComponentState( itWorstHealth->second.state );
}
*/

orca::ComponentState toComponentState( const hydroiceutil::LocalComponentStatus &local )
{    
    bool anybodyIdle = false;
    bool anybodyInitialising = false;
    bool anybodyFinalising = false;
    bool anybodyShutdown = false;
    bool everybodyWorking = true;
    
    hydroiceutil::LocalSubsystemStatusMap::const_iterator it;
    
    for ( it=local.subsystems.begin(); it!=local.subsystems.end(); ++it ) 
    {
        const hydroiceutil::LocalSubsystemStatus& status = it->second;
        switch ( status.state ) 
        {
            case gbxutilacfr::SubsystemIdle:
                anybodyIdle = true;
                break;
            case gbxutilacfr::SubsystemInitialising:
                anybodyInitialising = true;
                break;
            case gbxutilacfr::SubsystemWorking:
                // do nothing
                break;
            case gbxutilacfr::SubsystemFinalising:
                if (it->second.type != gbxutilacfr::SubsystemEarlyExit) {
                    anybodyFinalising = true;
                }
                break;
            case gbxutilacfr::SubsystemShutdown:
                if (it->second.type != gbxutilacfr::SubsystemEarlyExit) {
                    anybodyShutdown = true;
                }
                break;
        }
    }

    if ( anybodyIdle || anybodyInitialising || anybodyFinalising || anybodyShutdown )
        everybodyWorking = false;
    
    orca::ComponentState state;
    
    if ( everybodyWorking ) {
        state = orca::CompWorking;
    } 
    else {
        // alexm: does this produce the desired result?
        if (anybodyIdle)
            state = orca::CompInitialising;
        if (anybodyInitialising)
            state = orca::CompInitialising;
        if (anybodyFinalising)
            state = orca::CompFinalising;
        if (anybodyShutdown)
            state = orca::CompFinalising;
    }

    return state;
}

bool toComponentStall( const hydroiceutil::LocalComponentStatus &local )
{    
    hydroiceutil::LocalSubsystemStatusMap::const_iterator it;    
    for ( it=local.subsystems.begin(); it!=local.subsystems.end(); ++it ) 
    {
        if ( it->second.isStalled )
            return true;
    }
    return false;
}

void convertStateBased( const hydroiceutil::LocalComponentStatus &local, 
                        const orca::FQComponentName              &name, 
                        orca::ComponentStatus                    &net )
{
    // initialisation
    net.timeUp = 0;
    
    // convert name and publishInterval
    net.name = name;
    net.publishIntervalSec = local.publishIntervalSec;
    
    // if no subsystems exist, the state is the infrastructure state, health is ok, no stall
    // and we return
    if (local.subsystems.size()==0) {
        net.state = toComponentState( local.infrastructure );
        net.health = orca::CompOk;
        net.isStalled = false;
        return;
    }
    
    // convert subsystems
    convert( local.subsystems, net.subsystems );
    
    //
    // ComponentState
    //
    // Infrastructure has priority
    if ( local.infrastructure != gbxutilacfr::SubsystemWorking )
        net.state = toComponentState( local.infrastructure );
    else
        net.state = toComponentState( local );
    
    //
    // ComponentHealth (independent of ComponentState)
    //
    gbxutilacfr::SubsystemHealth worstHealth = gbxutilacfr::SubsystemOk;  
  
    hydroiceutil::LocalSubsystemStatusMap::const_iterator it;
    for ( it=local.subsystems.begin(); it!=local.subsystems.end(); ++it ) 
    {
        const hydroiceutil::LocalSubsystemStatus& status = it->second;
        if ( status.health >= worstHealth )
            worstHealth = status.health;
    }
    net.health = toComponentHealth( worstHealth );

    //
    // Component Stall
    //
    net.isStalled = toComponentStall( local );
}

} // end of namespace
    
/////////////////////////////////////////

ComponentStatusAggregator::ComponentStatusAggregator( AggregatorPolicy policy ) :
    policy_(policy)
{
}

void 
ComponentStatusAggregator::convert( const hydroiceutil::LocalComponentStatus &local,
                                    const orca::FQComponentName              &name, 
                                    orca::ComponentStatus                    &net )
{
    switch (policy_)
    {
//         case AggregatorPolicyWorstHealth:
//             convertWorstHealth( local, name, net );
//             break;
        case AggregatorPolicyStateBased:
            convertStateBased( local, name, net );
            break;
        default:
            assert( false && "unkown policy" );    
    }
    
}

}
}
