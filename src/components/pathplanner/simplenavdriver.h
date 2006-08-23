/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA2_PATHPLANNER_SIMPLENAVDRIVER_H
#define ORCA2_PATHPLANNER_SIMPLENAVDRIVER_H

#include "algodriver.h"

#include <orcapathplan/ipathplanner2d.h>
#include <orcaogmap/orcaogmap.h>

namespace pathplanner
{
    
class SimpleNavDriver : public AlgoDriver 
{

public:
    SimpleNavDriver( orca::OgMapDataPtr &ogMapDataPtr,
                     double robotDiameterMetres,
                     double traversabilityThreshhold,
                     bool doPathOptimization );
    
    ~SimpleNavDriver();
    
    // Computes the path
    virtual void computePath(   const orca::PathPlanner2dTaskPtr  & taskPtr,
                                const orca::PathPlanner2dDataPtr  & pathDataPtr );
private:
    
    orcaogmap::OgMap ogMap_;
    orcapathplan::IPathPlanner2d  *pathPlanner_;

};

} // namespace

#endif
