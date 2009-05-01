/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_COMONENTSTATUS_AGGREGATOR_H
#define ORCAICE_COMONENTSTATUS_AGGREGATOR_H

#include <hydroiceutil/localstatus.h>
#include <orca/status.h>

namespace orcaice
{
namespace detail
{
    
enum AggregatorPolicy
{

    // The subsystem with the "worst" health and its corresponding state define the component state and health
    // alexm: disabled because it's not currently used.
//     AggregatorPolicyWorstHealth,
    
    // Component health and state are chosen independently: health is the worst health in the system.
    // State: 
    //  if any subsystem is Idle/Initialising -> component state is set to CompInitialising
    //  if all subsystems are Working -> component state is CompWorking
    //  if any subsystem is Finalising/Shutdown and 
    //     none of those are *not* of type SubsystemEarlyExit -> component is set to CompFinalising
    //     else, -> the state of those subsystems are ignored
    AggregatorPolicyStateBased
};

// 
// Aggregates a Component Status based on the status of the component's subsystems
//
class ComponentStatusAggregator
{

public:
    
    ComponentStatusAggregator( AggregatorPolicy policy = AggregatorPolicyStateBased );
            
    void convert( const hydroiceutil::LocalComponentStatus &from,
                  const orca::FQComponentName              &name, 
                  orca::ComponentStatus                    &to );
            
private:
    
    AggregatorPolicy policy_;
    
    
};

}
}

#endif

