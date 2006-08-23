/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "skeletondriver.h"
#include <orcapathplan/skeletonpathplanner.h>
#include <orcapathplan/sparseskeletonpathplanner.h>
#include <orcamisc/orcamisc.h>
#include <orcapathplan/sparseskel.h>
#include <orcaice/orcaice.h>
#include <iostream>

using namespace std;
using namespace orcapathplan;
using namespace orcaice;

namespace pathplanner {
    
SkeletonDriver::SkeletonDriver( orca::OgMapDataPtr &ogMapDataPtr,
                                SkeletonGraphicsI* skelGraphicsI,
                                double robotDiameterMetres,
                                double traversabilityThreshhold,
                                bool   doPathOptimization,
                                bool   useSparseSkeleton )
    : skelGraphicsI_(skelGraphicsI)
{
    cout<<"TRACE(skeletondriver.cpp): SkeletonDriver()" << endl;
    convert( ogMapDataPtr, ogMap_ );

    orcamisc::CpuStopwatch watch(true);
    if ( !useSparseSkeleton )
    {
        try {
            orcapathplan::SkeletonPathPlanner *skelPathPlanner = 
                new orcapathplan::SkeletonPathPlanner( ogMap_,
                                                   robotDiameterMetres,
                                                   traversabilityThreshhold,
                                                   doPathOptimization );
            pathPlanner_ = skelPathPlanner;
            skelGraphicsI_->localSetSkel( ogMap_, &(skelPathPlanner->skeleton()) );
        }
        catch ( orcapathplan::Exception &e )
        {
            std::stringstream ss;
            ss << "Error trying to construct a skeletonpathplanner";
            throw orcapathplan::Exception( ss.str() );
        } 

    }
    else
    {
        // NOTE: display the dense skel first (for debugging).
        const bool displayDenseSkelFirst = false;
        if ( displayDenseSkelFirst )
        {
            orcapathplan::SkeletonPathPlanner *temp = 
                new orcapathplan::SkeletonPathPlanner( ogMap_,
                                                       robotDiameterMetres,
                                                       traversabilityThreshhold,
                                                       doPathOptimization );
            skelGraphicsI_->localSetSkel( ogMap_, &(temp->skeleton()) );
        }

        try {
            orcapathplan::SparseSkeletonPathPlanner *skelPathPlanner = 
                new orcapathplan::SparseSkeletonPathPlanner( ogMap_,
                                                            robotDiameterMetres,
                                                            traversabilityThreshhold,
                                                            doPathOptimization );
            pathPlanner_ = skelPathPlanner;
            skelGraphicsI_->localSetSkel( ogMap_,
                                          &(skelPathPlanner->denseSkel()),
                                          &(skelPathPlanner->sparseSkel()) );
        }
        catch( orcapathplan::Exception &e )
        {
            std::stringstream ss;
            ss << "Error trying to construct a sparseskeletonpathplanner";
            throw orcapathplan::Exception( ss.str() );
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
    orca::Waypoint2d *startWp = &(taskPtr->coarsePath[0]);
    
    for (unsigned int i=1; i<coarsePath.size(); i++)
    {
        orca::Waypoint2d *goalWp = &(coarsePath[i]);
        orcapathplan::Cell2DVector pathSegment;

        orcamisc::CpuStopwatch watch(true);
        assert(pathPlanner_!=0);
        try {
            int startX, startY, endX, endY;
            ogMap_.getCellIndices( startWp->target.p.x, startWp->target.p.y, startX, startY );
            ogMap_.getCellIndices( goalWp->target.p.x,  goalWp->target.p.y,  endX,   endY );
            pathPlanner_->computePath( startX,
                                       startY,
                                       endX,
                                       endY,
                                       pathSegment );
        }
        catch ( orcapathplan::Exception &e )
        {
            std::stringstream ss;
            ss << "Error planning path segment from (" 
               << startWp->target.p.x <<","<<startWp->target.p.y << ") to ("
               << goalWp->target.p.x << ","<<goalWp->target.p.y<<"): "
               << e.what()
               << endl;
            
            throw orcapathplan::Exception( ss.str() );
        }
        cout<<"TRACE(skeletondriver.cpp): computing path segment took " << watch.elapsedSeconds() << "s" << endl;

        // ====== Add waypoint parameters ================================
        // ====== Different options could be implemented and chosen and runtime (via .cfg file)
        vector<WaypointParameter> wpParaVector;
        addWaypointParameters( wpParaVector, startWp, goalWp, pathSegment.size() );
        // ===============================================================
        
        // ===== Append to the pathDataPtr which contains the entire path  ========
        orcapathplan::Result result = orcapathplan::PathOk;
        orcapathplan::convert( ogMap_, pathSegment, wpParaVector, result, pathDataPtr );
        // ========================================================================
        
        // set last goal cell as new start cell
        startWp = goalWp;
    }
}

void
SkeletonDriver::addWaypointParameters(  vector<WaypointParameter> &wpParaVector, 
                                        orca::Waypoint2d *startWp, 
                                        orca::Waypoint2d *goalWp, 
                                        int numSegments )
{
        WaypointParameter wpPara;
        
        wpPara.distanceTolerance = goalWp->distanceTolerance;
        wpPara.headingTolerance = goalWp->headingTolerance;
        wpPara.maxApproachSpeed = goalWp->maxApproachSpeed;
        wpPara.maxApproachTurnrate = goalWp->maxApproachTurnrate;
        double secondsTilGoal = orcaice::timeDiffAsDouble(goalWp->timeTarget, startWp->timeTarget);
        assert( secondsTilGoal >= 0 && "Timestamp difference between goal and start is negative" );
        double deltaSec = secondsTilGoal/(double)numSegments;
            
        for (int i=0; i<numSegments; i++)
        {
            if (i==0) {
                wpPara.timeTarget = toOrcaTime( timeAsDouble( startWp->timeTarget ) + deltaSec );
            } else {                        
                wpPara.timeTarget = toOrcaTime( timeAsDouble( wpParaVector[i-1].timeTarget ) + deltaSec );
            }
            wpParaVector.push_back( wpPara );
        }
}

}
