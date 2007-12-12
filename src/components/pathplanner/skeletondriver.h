/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef ORCA_PATHPLANNER_SKELETONDRIVER_H
#define ORCA_PATHPLANNER_SKELETONDRIVER_H

#include "algodriver.h"
#include "genericdriver.h"
#include <orcaice/orcaice.h>
#include <hydropathplan/hydropathplan.h>
#include <orca/ogmap.h>
#include <orcaogmap/orcaogmap.h>
#include <vector>

namespace pathplanner {

class SkeletonGraphicsI;
//
// The 'skeletonnav' driver.
//
// @author Alex Brooks, Tobias Kaupp
//
class SkeletonDriver : public AlgoDriver
{

public: 

    SkeletonDriver( const  hydroogmap::OgMap &ogMap,
                    double robotDiameterMetres,
                    double traversabilityThreshhold,
                    bool   doPathOptimization,
                    bool   jiggleWaypointsOntoClearCells,
                    bool   useSparseSkeleton,
                    bool   addExtraSparseSkelNodes,
                    double sparseSkelExtraNodeResolution,
                    const  hydropathplan::CostEvaluator &costEvaluator,
                    const  orcaice::Context &context);
    ~SkeletonDriver();
    
    virtual void computePath( const orca::PathPlanner2dTask& task,
                              orca::PathPlanner2dData& pathData );

private: 
    
    #ifdef QT4_FOUND
    void setGraphics( const hydroogmap::OgMap &ogMap,
                      double            robotDiameterMetres,
                      double            traversabilityThreshhold,
                      bool              doPathOptimization );
    #endif
    
    hydropathplan::IPathPlanner2d *pathPlanner_;
    GenericDriver                *genericDriver_;
    SkeletonGraphicsI            *skelGraphicsI_;
    
    const orcaice::Context context_;

    bool   useSparseSkeleton_; 
};

}

#endif
