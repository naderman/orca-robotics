/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef SIMPLENAVPATHPLANNER_H
#define SIMPLENAVPATHPLANNER_H

#include <orcapathplan/ipathplanner2d.h>
#include <orcapathplan/pathplanutils.h>
#include <orcaogmap/orcaogmap.h>

namespace orcapathplan {

//!
//! @author Tobias Kaupp
//!
class SimpleNavPathPlanner : public IPathPlanner2d
{

public: 

    SimpleNavPathPlanner( const orcaogmap::OgMap &ogMap,
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

    const orcaogmap::OgMap &ogMap_;

    const double robotDiameterMetres_;
    const double traversabilityThreshhold_;
    const bool   doPathOptimization_;
};

}

#endif
