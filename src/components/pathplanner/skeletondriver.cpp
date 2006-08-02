#include "skeletondriver.h"
#include <orcapathplan/orcapathplan.h>
#include <orcapathplan/skeletonpathplanner.h>
#include <orcapathplan/sparseskeletonpathplanner.h>
#include <orcamisc/orcamisc.h>
#include <orcapathplan/sparseskel.h>
#include <iostream>

using namespace std;

namespace pathplanner {

SkeletonDriver::SkeletonDriver( orca::OgMapDataPtr &ogMapDataPtr,
                                SkeletonGraphicsI* skelGraphicsI,
                                double robotDiameterMetres,
                                double traversabilityThreshhold,
                                bool   doPathOptimization,
                                bool   useSparseSkeleton )
    : skelGraphicsI_(skelGraphicsI),
      robotDiameterMetres_(robotDiameterMetres),
      doPathOptimization_(doPathOptimization)
{
    cout<<"TRACE(skeletondriver.cpp): SkeletonDriver()" << endl;
    convert( ogMapDataPtr, ogMap_ );

    orcamisc::CpuStopwatch watch(true);
    if ( !useSparseSkeleton )
    {
        orcapathplan::SkeletonPathPlanner *skelPathPlanner = 
            new orcapathplan::SkeletonPathPlanner( ogMap_,
                                                   robotDiameterMetres,
                                                   traversabilityThreshhold );
        pathPlanner_ = skelPathPlanner;
        skelGraphicsI_->localSetSkel( ogMap_, &(skelPathPlanner->skeleton()) );
    }
    else
    {
        // NOTE: Un-comment to display the dense skel first.
        //orcapathplan::SkeletonPathPlanner *temp = 
        //    new orcapathplan::SkeletonPathPlanner( ogMap_,
        //                                           robotDiameterMetres,
        //                                           traversabilityThreshhold );
        //skelGraphicsI_->localSetSkel( ogMap_, &(temp->skeleton()) );
        // /NOTE

        orcapathplan::SparseSkeletonPathPlanner *skelPathPlanner = 
            new orcapathplan::SparseSkeletonPathPlanner( ogMap_,
                                                         robotDiameterMetres,
                                                         traversabilityThreshhold );
        pathPlanner_ = skelPathPlanner;

        try {
            skelGraphicsI_->localSetSkel( ogMap_,
                                          &(skelPathPlanner->denseSkel()),
                                          &(skelPathPlanner->sparseSkel()) );
        }
        catch ( Ice::MemoryLimitException &e )
        {
            cout<<"TRACE(skeletondriver.cpp): Caught: " << e << endl;
            cout<<"TRACE(skeletondriver.cpp): This means the dense skel is too big to send out." << endl;
        }
    }
    cout<<"TRACE(skeletondriver.cpp): Creating skeleton took " << watch.elapsedSeconds() << "s" << endl;
}

SkeletonDriver::~SkeletonDriver()
{
    if ( pathPlanner_ ) delete pathPlanner_;
}

void 
SkeletonDriver::computePath( const orca::OgMapDataPtr         &ogMapDataPtr,
                             const orca::PathPlanner2dTaskPtr &taskPtr,
                             const orca::PathPlanner2dDataPtr &pathDataPtr )
{
    // for each waypoint in the coarse path:
    orca::Path2d &coarsePath = taskPtr->coarsePath;
    orca::Waypoint2d *startWp = &(taskPtr->start);
    for (unsigned int i=0; i<coarsePath.size(); i++)
    {
        orca::Waypoint2d *goalWp = &(coarsePath[i]);
        orcapathplan::Cell2DVector pathSegment;

        orcamisc::CpuStopwatch watch(true);
        assert(pathPlanner_!=0);
        try {
            pathPlanner_->computePath( startWp->target.p.x,
                                       startWp->target.p.y,
                                       goalWp->target.p.x,
                                       goalWp->target.p.y,
                                       pathSegment );
        }
        catch ( orcapathplan::Exception &e )
        {
            std::stringstream ss;
            ss << "Error planning path segment from (" 
               << startWp->target.p.x <<","<<startWp->target.p.y << ") to ("
               << goalWp->target.p.x << ","<<goalWp->target.p.y<<"): "
               << e.what();
            throw orcapathplan::Exception( ss.str() );
        }
        cout<<"TRACE(skeletondriver.cpp): computing path segment took " << watch.elapsedSeconds() << "s" << endl;

        if ( doPathOptimization_ )
        {
            cout<<"TRACE(skeletondriver.cpp): AlexB: doPathOptimization turned off for now, due to a bug..." << endl;
//             // separate full path into a optimized short path
//             orcapathplan::Cell2DVector waycells;    
//             orcapathplan::optimizePath( ogMap_, pathSegment, waycells, robotDiameterMetres_ );
//             pathSegment = waycells;
        }

        // ====== Convert to an Orca object in global coordinate system. =====
        // ====== Will append latest path to the total pathDataPtr. ==========
        // ====== Not all data fields are filled in (e.g.tolerances) =========
        if (i==0)
        {
            // the first time we'll have to insert the start cell
            int cx, cy;
            ogMap_.getCellIndices( startWp->target.p.x, startWp->target.p.y, cx, cy );
            orcapathplan::Cell2D startCell( cx, cy );
            pathSegment.insert(pathSegment.begin(),startCell);
        }
        orcapathplan::Result result = orcapathplan::PathOk;
        orcapathplan::convert( ogMap_, pathSegment, result, pathDataPtr );
        // ==================================================================
        
        // set last goal cell as new start cell
        startWp = goalWp;
    }
}

}
