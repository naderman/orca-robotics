/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alexei Makarenko, George Mathews, Matthew Ridley
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef _OGMAP_RAYTRACE_H_
#define _OGMAP_RAYTRACE_H_


#include <vector>
#include <orca/bros1.h>
#include <hydroutil/mathdefs.h>
#include <orcapathplan/cell2d.h>
#include <orcaogmap/ogmap.h>


namespace laser2og
{ 
    /*!
    Returns a list of cells along a straight line from one cell to another.
     */
    std::vector<orcapathplan::Cell2D> rayTrace( const orcapathplan::Cell2D & startCell, const orcapathplan::Cell2D & endCell );
  
    /*!
        Returns a list of cells along a straight line from one point to another.
        Both points startPoint and endPoint must be in the world coordinate system.
     */
    std::vector<orcapathplan::Cell2D> rayTrace(const orcaogmap::CartesianPoint2d & startPoint,
				       const orcaogmap::CartesianPoint2d & endPoint,
                                        const double originX, const double originY,
                                        const double mapResX, const double mapResY = -1 ) ;
}
  
#endif

