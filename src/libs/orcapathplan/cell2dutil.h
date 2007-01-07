/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_CELL2D_UTIL_H
#define ORCA2_CELL2D_UTIL_H

#include "cell2d.h"
#include <orcaogmap/ogmaputils.h>

namespace orcapathplan
{

/*!
         GRID COORDINATE SYSTEM                 GRID SUBSCRIPTING                  GRID INDEXING
           _________________                   _________________               _________________
          |                 |      sizeY-1    |  ^              |             |                 | 
          |                 |         ^       |  |              |             |                 | 
 y-axis   |                 |         .       |  |              |             |                 |
   ^      |                 |         . indY  |  |sizeY         |             |                 |
   |      |                 |         .       |  |              |             |                 |
   |----->|                 |         .       |  |    sizeY     |             |----> index      |
   |  oX  |                 |         .       |<-|------------->|             |---------------->|
   |      |_________________|         0       |__v______________|             |_________________|
   |         ^                               
   |         | oY                                     indX                  index: 0 -> sizeX * sizeY - 1
   |_________|_____>  x-axis                  0 - - - - - - -> sizeX-1

   
 */

    //! convers a point in 2d space to a cell reference
    //! @relatesalso Grid2D
    Cell2D point2cell( const orcaogmap::CartesianPoint2d & pt, const double & originX, const double & originY,
                       const double & resX, const double & resY=-1 );

    //! convers a cell reference to a point in 2d space
    //! @relatesalso Grid2D
    orcaogmap::CartesianPoint2d cell2point( const Cell2D c, const double originX, const double originY,
                        const double resX, const double resY=-1 );

    //! Find the geometric distance between two cells
    //! @relatesalso CartesianPoint2d
    double euclideanDistance( const orcaogmap::CartesianPoint2d p1, const orcaogmap::CartesianPoint2d p2 );


} // namespace

#endif

