/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA2_PATHPLANNER_MAIN_THREAD_H
#define ORCA2_PATHPLANNER_MAIN_THREAD_H

#include <hydroutil/safethread.h>
#include <orcaice/context.h>
#include <hydroutil/proxy.h>

#include <orca/ogmap.h>
#include <orca/pathplanner2d.h>
#include <orca/qgraphics2d.h>
#include <hydropathplan/hydropathplan.h>
#include <memory>

namespace pathplanner
{

class AlgoDriver;
class SkeletonGraphicsI;
class PathPlanner2dI;

class MainThread : public hydroutil::SafeThread
{

public: 

    MainThread( const orcaice::Context & context );
    ~MainThread();

    virtual void walk();

private:

    // generic inerface to the algorithm
    std::auto_ptr<AlgoDriver> driver_;

    // we have to keep the ogmap as member variable,
    // otherwise they get out of scope if we pass references around
    hydroogmap::OgMap ogMap_;

    PathPlanner2dI* pathPlannerI_;
    hydroutil::Proxy<orca::PathPlanner2dTask>* pathPlannerTaskProxy_;
    hydroutil::Proxy<orca::PathPlanner2dData>* pathPlannerDataProxy_;
    
    void initNetwork();
    void initDriver();
    
    orcaice::Context context_;
    
    // cost of traversing cells (only used for skeleton variants)
    std::auto_ptr<hydropathplan::CostEvaluator> costEvaluator_;
};

} // namespace

#endif
