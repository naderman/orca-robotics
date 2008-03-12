/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef MAIN_THREAD_H
#define MAIN_THREAD_H

#include <hydroiceutil/subsystemthread.h>
#include <orcaice/context.h>
#include <hydroiceutil/buffer.h>

#include <hydropathplan/hydropathplan.h>
#include <memory>
#include "driver.h"
#include "pathplanner2dI.h"

namespace pathplanner
{

class MainThread : public hydroiceutil::SubsystemThread
{

public: 

    MainThread( const orcaice::Context & context );

    virtual void walk();

private:

    // inerface to the algorithm and its driver
    std::auto_ptr<hydropathplan::IPathPlanner2d> pathPlanner_;
    std::auto_ptr<Driver>                        driver_;

    // we have to keep the ogmap as member variable,
    // otherwise they get out of scope if we pass references around
    hydroogmap::OgMap ogMap_;

    PathPlanner2dI* pathPlannerI_;
    hydroiceutil::Buffer<orca::PathPlanner2dTask> pathPlannerTaskBuffer_;
    
    void initNetwork();
    void initDriver();
    
    orcaice::Context context_;
    
    // cost of traversing cells (only used for skeleton variants)
    std::auto_ptr<hydropathplan::CostEvaluator> costEvaluator_;
};

} // namespace

#endif
