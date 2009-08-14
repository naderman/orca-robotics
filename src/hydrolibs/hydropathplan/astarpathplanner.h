/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef ASTAR_PATHPLANNER_H
#define ASTAR_PATHPLANNER_H

#include <hydropathplan/ipathplanner2d.h>
#include <hydropathplan/hydropathplan.h>
#include <hydroogmap/hydroogmap.h>

namespace hydropathplan {

// fwd decl
namespace astar {
    class AStar;
}

//!
//! @author Tobias Kaupp
//!
class AStarPathPlanner : public IPathPlanner2d
{

public: 

    AStarPathPlanner( const hydroogmap::OgMap ogMap,
                      double robotDiameterMetres,
                      double traversabilityThreshhold,
                      bool   doPathOptimization );

    ~AStarPathPlanner();

    //!
    //! Warning: may throw 'hydropathplan::Exception's
    //!
    virtual void computePath( int           startX,
                              int           startY,
                              int           endX,
                              int           endY,
                              Cell2DVector &path ) const;

private: 

    void checkInputs( int           startX,
                      int           startY,
                      int           endX,
                      int           endY ) const;

    hydroogmap::OgMap             ogMap_;
    hydropathplan::astar::AStar  *aStar_;

    const double robotDiameterMetres_;
    const double traversabilityThreshhold_;
    const bool   doPathOptimization_;
};

}

#endif
