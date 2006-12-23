/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA2_PATHPLANNER_ALGORITHM_HANDLER_H
#define ORCA2_PATHPLANNER_ALGORITHM_HANDLER_H

#include <orcaice/thread.h>
#include <orcaice/context.h>
#include <orcaice/ptrproxy.h>

#include <orca/ogmap.h>
#include <orca/pathplanner2d.h>
#include <orca/qgraphics2d.h>

namespace pathplanner
{

class AlgoDriver;
class SkeletonGraphicsI;
class PathPlanner2dI;

class AlgoHandler : public orcaice::Thread
{

public: 

    AlgoHandler( const orcaice::Context & context );
    ~AlgoHandler();

    virtual void run();

private:

    // generic inerface to the algorithm
    AlgoDriver* driver_;

    orca::OgMapPrx ogMapPrx_;
    orca::OgMapPrx hazardMapPrx_;

    PathPlanner2dI* pathPlannerI_;
    orcaice::Proxy<orca::PathPlanner2dTask>* pathPlannerTaskProxy_;
    orcaice::Proxy<orca::PathPlanner2dData>* pathPlannerDataProxy_;
    
    void initNetwork();
    void initDriver();
    
    orcaice::Context context_;
    
    int useHazardMap_;

};

} // namespace

#endif
