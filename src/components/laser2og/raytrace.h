/*
  \file raytrace.h
  \date Fri Nov 15 2002
  \brief Misc conversion tools used in ACFR ASN project.
  \author Copyright (C) 2002-2003 by Alex Makarenko, alexei@cas.edu.au
  
  \date 1 may 2005 George Mathews - converted to asntools and added to the orca project
  \date 1 april 2006 Matthew Ridley - conversion to orca2/bruce2
*/


#ifndef _OGMAP_RAYTRACE_H_
#define _OGMAP_RAYTRACE_H_


#include <vector>
#include <orca/bros1.h>
#include <orcaobj/mathdefs.h>

#include <orcapathplan/cell2d.h>
#include <orcaogmap/ogmaputils.h>


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

