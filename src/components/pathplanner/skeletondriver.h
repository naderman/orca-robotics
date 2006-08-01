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
                    double traversabilityThreshhold );
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

};

}

#endif
