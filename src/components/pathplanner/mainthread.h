/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef MAIN_THREAD_H
#define MAIN_THREAD_H

#include <hydroutil/subsystemthread.h>
#include <orcaice/context.h>
#include <hydroutil/buffer.h>

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

class MainThread : public hydroutil::SubsystemThread
{

public: 

    MainThread( const orcaice::Context & context );

    virtual void walk();

private:

    // generic inerface to the algorithm
    std::auto_ptr<AlgoDriver> driver_;

    // we have to keep the ogmap as member variable,
    // otherwise they get out of scope if we pass references around
    hydroogmap::OgMap ogMap_;

    PathPlanner2dI* pathPlannerI_;
    hydroutil::Buffer<orca::PathPlanner2dTask> pathPlannerTaskBuffer_;
    
    void initNetwork();
    void initDriver();
    
    orcaice::Context context_;
    
    // cost of traversing cells (only used for skeleton variants)
    std::auto_ptr<hydropathplan::CostEvaluator> costEvaluator_;
};

} // namespace

#endif
