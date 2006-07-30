#include "skeletondriver.h"
#include <orcapathplan/orcapathplan.h>
#include <iostream>

using namespace std;

namespace pathplanner {

SkeletonDriver::SkeletonDriver( orca::OgMapDataPtr &ogMapDataPtr,
                                SkeletonGraphicsI* skelGraphicsI,
                                double robotDiameterMetres,
                                double traversabilityThreshhold )
    : skelGraphicsI_(skelGraphicsI)
{
    convert( ogMapDataPtr, ogMap_ );
    cout<<"TRACE(skeletondriver.cpp): TODO: Deal with exceptions!!!" << endl;
    pathPlanner_ = new orcapathplan::SkeletonPathPlanner( ogMap_, robotDiameterMetres, traversabilityThreshhold );

    // Send the skeleton to the gui for debug reasons
    displaySkeleton( pathPlanner_->skeleton() );
}

SkeletonDriver::~SkeletonDriver()
{
    if ( pathPlanner_ ) delete pathPlanner_;
}

void
SkeletonDriver::displaySkeleton( const orcapathplan::Cell2DVector &skeleton )
{
    Point2dVector skelWorld;

    for (unsigned int i=0; i<skeleton.size(); i++)
    {
        float worldX, worldY;
        ogMap_.getWorldCoords( skeleton[i].x(),
                               skeleton[i].y(),
                               worldX,
                               worldY );
        Point2d worldPoint( worldX, worldY );
        skelWorld.push_back( worldPoint );
    }

    skelGraphicsI_->localSetData( skelWorld );
}

void 
SkeletonDriver::computePath( const orca::OgMapDataPtr         &ogMapDataPtr,
                             const orca::PathPlanner2dTaskPtr &taskPtr,
                             const orca::PathPlanner2dDataPtr &pathDataPtr )
{
    cout<<"TRACE(skeletondriver.cpp): TODO: Deal with exceptions!!!" << endl;

    // for each waypoint in the coarse path:
    orca::Path2d &coarsePath = taskPtr->coarsePath;
    orca::Waypoint2d *startWp = &(taskPtr->start);
    for (unsigned int i=0; i<coarsePath.size(); i++)
    {
        orca::Waypoint2d *goalWp = &(coarsePath[i]);
        orcapathplan::Cell2DVector pathSegment;
        pathPlanner_->computePath( startWp->target.p.x,
                                   startWp->target.p.y,
                                   goalWp->target.p.x,
                                   goalWp->target.p.y,
                                   pathSegment );

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
