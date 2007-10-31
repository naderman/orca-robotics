/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef SKELETONPATHPLANNER_H
#define SKELETONPATHPLANNER_H

#include <orcapathplan/ipathplanner2d.h>
#include <orcapathplan/pathplanutils.h>
#include <orcaogmap/orcaogmap.h>

namespace orcapathplan {

//!
//! @author Alex Brooks
//!
class SkeletonPathPlanner : public IPathPlanner2d
{

public: 

    //!
    //! Warning: may throw 'orcapathplan::Exception's
    //!
    SkeletonPathPlanner( const orcaogmap::OgMap &ogMap,
                         double robotDiameterMetres,
                         double traversabilityThreshhold,
                         bool   doPathOptimization,
                         const CostEvaluator &costEvaluator=DefaultCostEvaluator() );

    //!
    //! Warning: may throw 'orcapathplan::Exception's
    //!
    virtual void computePath( int           startX,
                              int           startY,
                              int           endX,
                              int           endY,
                              Cell2DVector &path ) const;

    // Useful for debugging.
    const Cell2DVector &skeleton()   const { return skel_; }
    const FloatMap     &distGrid()   const { return distGrid_; }

private: 

    void checkInputs( int           startX,
                      int           startY,
                      int           endX,
                      int           endY ) const;

    const orcaogmap::OgMap &ogMap_;

    FloatMap     costMap_;
    Cell2DVector skel_;
    FloatMap     distGrid_;

    const double robotDiameterMetres_;
    const double traversabilityThreshhold_;
    const bool   doPathOptimization_;
};

}

#endif
