/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCA_PATHPLANNER_SKELETONDRIVER_H
#define ORCA_PATHPLANNER_SKELETONDRIVER_H

#include "algodriver.h"
#include "skeletongraphicsI.h"
#include <orcapathplan/ipathplanner2d.h>
#include <orca/ogmap.h>
#include <orcaogmap/orcaogmap.h>

namespace pathplanner {

//
// The 'skeletonnav' driver.
//
// @author Alex Brooks
//
class SkeletonDriver : public AlgoDriver
{

public: 

    SkeletonDriver( orca::OgMapDataPtr &ogMapDataPtr,
                    SkeletonGraphicsI* skelGraphicsI,
                    double robotDiameterMetres,
                    double traversabilityThreshhold,
                    bool   doPathOptimization,
                    bool   useSparseSkeleton );
    ~SkeletonDriver();

    // Computes the path
    // (NOTE: ignores the input ogMap, using the one from the constructor instead...)
    virtual void computePath( const orca::OgMapDataPtr         &ogMapDataPtr,
                              const orca::PathPlanner2dTaskPtr &taskPtr,
                              const orca::PathPlanner2dDataPtr &pathDataPtr );

private: 

    SkeletonGraphicsI             *skelGraphicsI_;
    orcaogmap::OgMap               ogMap_;
    orcapathplan::IPathPlanner2d  *pathPlanner_;
    double                         robotDiameterMetres_;
};

}

#endif
