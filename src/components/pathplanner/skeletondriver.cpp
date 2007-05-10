/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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

#ifdef QT4_FOUND
    #include "skeletongraphicsI.h"
#endif

using namespace std;
using namespace orcaice;

namespace pathplanner {
    
SkeletonDriver::SkeletonDriver( orcaogmap::OgMap &ogMap,
                                double robotDiameterMetres,
                                double traversabilityThreshhold,
                                bool   doPathOptimization,
                                bool   useSparseSkeleton,
                                const orcapathplan::CostEvaluator &costEvaluator )
    : ogMap_(ogMap),
      skelGraphicsI_(NULL),
      robotDiameterMetres_(robotDiameterMetres),
      traversabilityThreshhold_(traversabilityThreshhold),
      doPathOptimization_(doPathOptimization),
      useSparseSkeleton_(useSparseSkeleton)
      
{
    cout<<"TRACE(skeletondriver.cpp): SkeletonDriver()" << endl;

    orcamisc::CpuStopwatch watch(true);
    if ( !useSparseSkeleton )
    {
        try {
            orcapathplan::SkeletonPathPlanner *skelPathPlanner = 
                new orcapathplan::SkeletonPathPlanner( ogMap_,
                                                       robotDiameterMetres,
                                                       traversabilityThreshhold,
                                                       doPathOptimization,
                                                       costEvaluator );
            pathPlanner_ = skelPathPlanner;
        }
        catch ( orcapathplan::Exception &e )
        {
            stringstream ss;
            ss << "Error trying to construct a skeletonpathplanner";
            throw orcapathplan::Exception( ss.str() );
        } 

    }
    else
    {
        try {
            orcapathplan::SparseSkeletonPathPlanner *skelPathPlanner = 
                new orcapathplan::SparseSkeletonPathPlanner( ogMap_,
                                                             robotDiameterMetres,
                                                             traversabilityThreshhold,
                                                             doPathOptimization,
                                                             costEvaluator );
            pathPlanner_ = skelPathPlanner;
        }
        catch( orcapathplan::Exception &e )
        {
            stringstream ss;
            ss << "Error trying to construct a sparseskeletonpathplanner";
            throw orcapathplan::Exception( ss.str() );
        }  
        catch ( Ice::MemoryLimitException &e )
        {
            stringstream ss;
            ss<<"Caught: " << e << " .This means the dense skel is too big to send out." << endl;
            throw orcapathplan::Exception( ss.str() );
        }
    }
    cout<<"TRACE(skeletondriver.cpp): Creating skeleton took " << watch.elapsedSeconds() << "s" << endl;
}

SkeletonDriver::~SkeletonDriver()
{
    if ( pathPlanner_ ) delete pathPlanner_;
}

#ifdef QT4_FOUND
void
SkeletonDriver::setGraphics( SkeletonGraphicsI* skelGraphicsI )
{
    assert(pathPlanner_!=NULL);

    skelGraphicsI_ = skelGraphicsI;
    
    if ( !useSparseSkeleton_ )
    {
        orcapathplan::SkeletonPathPlanner *skelPathPlanner = dynamic_cast<orcapathplan::SkeletonPathPlanner*>( pathPlanner_ );
        skelGraphicsI_->localSetSkel( ogMap_, &(skelPathPlanner->skeleton()) );
    }
    else
    {
        const bool displayDenseSkelFirst = false;
        if ( displayDenseSkelFirst )
        {
            orcapathplan::SkeletonPathPlanner *temp = 
                    new orcapathplan::SkeletonPathPlanner( ogMap_,
                    robotDiameterMetres_,
                    traversabilityThreshhold_,
                    doPathOptimization_ );
            skelGraphicsI_->localSetSkel( ogMap_, &(temp->skeleton()) );
        }
        
        
        orcapathplan::SparseSkeletonPathPlanner *sparseSkelPathPlanner = dynamic_cast<orcapathplan::SparseSkeletonPathPlanner*>( pathPlanner_ );
        skelGraphicsI_->localSetSkel( ogMap_,
                                      &(sparseSkelPathPlanner->denseSkel()),
                                      &(sparseSkelPathPlanner->sparseSkel()) );
    }
}
#endif

void 
SkeletonDriver::computePath( const orca::PathPlanner2dTask& task,
                             orca::PathPlanner2dData& pathData )
{
    // get the first waypoint from the coarse path and save as startWp
    const orca::Path2d &coarsePath = task.coarsePath;
    const orca::Waypoint2d *startWp = &(task.coarsePath[0]);
    const double firstHeading = startWp->target.o;
    
    // for each waypoint in the coarse path
    for (unsigned int i=1; i<coarsePath.size(); i++)
    {
        const orca::Waypoint2d *goalWp = &(coarsePath[i]);
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

        // ====== Set waypoint parameters ================================
        // ====== Different options could be implemented and chosen and runtime (via .cfg file)
        vector<orcapathplan::WaypointParameter> wpParaVector;
        setWaypointParameters( startWp, goalWp, pathSegment.size(), wpParaVector );
        // ===============================================================

        // ===== Append to the pathData which contains the entire path  ========
        orcapathplan::convert( ogMap_, pathSegment, wpParaVector, pathData, firstHeading );
        // ========================================================================
        
        // set last goal cell as new start cell
        startWp = goalWp;
    }
}

// This function has the following policy for adding waypoint parameters to the path consisting
// of a start point, intermediate points and an end point:
// (1) Start and end points use their settings as given
// (2) Intermediate points use the settings of the endpoint
// (3) The time to reach intermediate points is divided linearly between start and end point times
void
SkeletonDriver::setWaypointParameters( const orca::Waypoint2d *startWp, 
                                       const orca::Waypoint2d *goalWp, 
                                       int numSegments,
                                       vector<orcapathplan::WaypointParameter> &wpParaVector )
{
    wpParaVector.clear();

    orcapathplan::WaypointParameter wpPara;
    double secondsTilGoal = orcaice::timeDiffAsDouble(goalWp->timeTarget, startWp->timeTarget);
    assert( secondsTilGoal >= 0 && "Timestamp difference between goal and start is negative" );
    double deltaSec = secondsTilGoal/(double)numSegments;
            
    for (int i=0; i<numSegments; i++)
    {
        if (i==0) 
        {
            wpPara.distanceTolerance = startWp->distanceTolerance;
            wpPara.maxApproachSpeed = startWp->maxApproachSpeed;
            wpPara.maxApproachTurnrate = startWp->maxApproachTurnrate;
            wpPara.timeTarget = toOrcaTime( timeAsDouble( startWp->timeTarget ) );
            wpPara.headingTolerance = startWp->headingTolerance;
        } 
        else 
        {
            wpPara.distanceTolerance = goalWp->distanceTolerance;
            wpPara.maxApproachSpeed = goalWp->maxApproachSpeed;
            wpPara.maxApproachTurnrate = goalWp->maxApproachTurnrate;                        
            wpPara.timeTarget = toOrcaTime( timeAsDouble( wpParaVector[i-1].timeTarget ) + deltaSec );
            wpPara.headingTolerance = goalWp->headingTolerance;
        }
        wpParaVector.push_back( wpPara );
    }
}

}
