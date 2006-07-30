#ifndef ORCA_PATHPLANNER_SKELETONDRIVER_H
#define ORCA_PATHPLANNER_SKELETONDRIVER_H

#include "algodriver.h"
#include "skeletongraphicsI.h"
#include <orcapathplan/skeletonpathplanner.h>
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

    void displaySkeleton( const orcapathplan::Cell2DVector &skeleton );

    SkeletonGraphicsI                 *skelGraphicsI_;
    orcaogmap::OgMap                   ogMap_;
    orcapathplan::SkeletonPathPlanner *pathPlanner_;

};

}

#endif
