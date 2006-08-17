/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA2_GOALPLANNER_ALGORITHM_HANDLER_H
#define ORCA2_GOALPLANNER_ALGORITHM_HANDLER_H

#include <orcaice/thread.h>
#include <orcaice/context.h>
#include <orcaice/ptrproxy.h>
 
#include <orca/localise2d.h>
#include <orca/pathplanner2d.h>
#include <orca/pathfollower2d.h>

namespace goalplanner
{

class PathFollower2dI;

class AlgoHandler : public orcaice::Thread
{

public: 

    AlgoHandler( const orcaice::Context & context );
    ~AlgoHandler();

    virtual void run();

private:
    
    // required interface to localiser
    orca::Localise2dPrx localise2dPrx_;
    
    // localisation comes in through icestorm
    orca::Localise2dConsumerPrx localiseConsumerPrx_;
    
    // required interface to localnav
    orca::PathFollower2dPrx localNavPrx_;
    
    // required interface to pathplanner
    orca::PathPlanner2dPrx pathplanner2dPrx_;
    
    // task proxy passed to pathplanner which receives the computed path
    orca::PathPlanner2dConsumerPrx taskPrx_;
    
    // buffer which stores computed path from pathplanner
    orcaice::PtrProxy<orca::PathPlanner2dDataPtr> computedPathBuffer_;
    
    // buffer that stores current localisation data
    orcaice::PtrProxy<orca::Localise2dDataPtr> localiseDataBuffer_;
        
    // ========== provided pathfollower interface (incoming paths) ===============
    PathFollower2dI* incomingPathI_;
    orca::PathFollower2dConsumerPrx pathPublisher_;
    
    orcaice::PtrProxy<orca::PathFollower2dDataPtr> incomingPathBuffer_;

    // Used by the PathFollowerI to inform of activation commands
    orcaice::Proxy<orca::Time>                     activationPipe_;
    // ===========================================================================
    
    void initNetwork();
     
    orcaice::Context context_;

};

} // namespace

#endif
