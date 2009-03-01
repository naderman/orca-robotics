/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef MAIN_THREAD_H
#define MAIN_THREAD_H

#include <orcaice/subsystemthread.h>
#include <orcaice/context.h>
#include <gbxsickacfr/gbxiceutilacfr/buffer.h>
#include <hydrodll/dynamicload.h>

#include <hydropathplan/hydropathplan.h>
#include <memory>
#include "driver.h"
#include "pathplanner2dI.h"

namespace pathplanner
{

class MainThread : public orcaice::SubsystemThread
{

public: 

    MainThread( const orcaice::Context & context );

private:

    // from SubsystemThread
    virtual void initialise();
    virtual void work();

    void initNetwork();
    void initDriver();
    
    // The library that contains the driver factory (must be declared first so it's destructed last!!!)
    std::auto_ptr<hydrodll::DynamicallyLoadedLibrary> driverLib_;
    // Generic hydro driver
    std::auto_ptr<hydrointerfaces::PathPlanner2d> hydroDriver_;

    // Configuration for the hydro driver
    std::auto_ptr<hydrointerfaces::PathPlanner2d::Config> hydroDriverConfig_;
    // Storage for the graphics publisher
    std::auto_ptr<hydrointerfaces::PathPlanner2d::QGraphics2dPublisher> graphicsPublisher_;

    // wraps the hydro driver
    std::auto_ptr<Driver>                        driver_;

    // we have to keep the ogmap as member variable,
    // otherwise they get out of scope if we pass references around
    hydroogmap::OgMap ogMap_;

    PathPlanner2dI* pathPlannerI_;
    gbxiceutilacfr::Buffer<orca::PathPlanner2dTask> pathPlannerTaskBuffer_;
    
    orcaice::Context context_;
    
    // cost of traversing cells (only used for skeleton variants)
    std::auto_ptr<hydropathplan::CostEvaluator> costEvaluator_;
};

} // namespace

#endif
