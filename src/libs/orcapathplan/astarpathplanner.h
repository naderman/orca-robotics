/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ASTAR_PATHPLANNER_H
#define ASTAR_PATHPLANNER_H

#include <orcapathplan/ipathplanner2d.h>
#include <orcapathplan/orcapathplan.h>
#include <orcaogmap/orcaogmap.h>

namespace orcapathplan {

//!
//! @author Tobias Kaupp
//!
class AStarPathPlanner : public IPathPlanner2d
{

public: 

    AStarPathPlanner( const orcaogmap::OgMap ogMap,
                      double robotDiameterMetres,
                      double traversabilityThreshhold,
                      bool   doPathOptimization );

    //!
    //! Warning: may throw 'orcapathplan::Exception's
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

    orcaogmap::OgMap ogMap_;
    orcapathplan::AStar* aStar_;

    const double robotDiameterMetres_;
    const double traversabilityThreshhold_;
    const bool   doPathOptimization_;
};

}

#endif
