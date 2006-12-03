/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

/*
  \file cell2dutil.cpp
  \date Thu Apr 21 2005
  \author George Mathews, g.mathews@cas.edu.au
*/

#include <orcaobj/mathdefs.h>  // for CHECK_LIMITS(max_x, x, min_x)
#include "cell2dutil.h"

using namespace orcapathplan;
using namespace orcaogmap;


Cell2D orcapathplan::point2cell( const CartesianPoint2d & pt, const double & originX, const double & originY,
                        const double & resX, const double & resY )
{
    double tmpResY = (resY > 0 ?  resY : resX); 
    return Cell2D( (int)floor( ( pt.x-originX ) / resX + 0.5 ) , (int)floor( ( pt.y-originY ) / tmpResY + 0.5 ) );
}

CartesianPoint2d orcapathplan::cell2point( const Cell2D c, const double originX, const double originY,
                       const double resX, const double resY )
{
    double tmpResY = (resY > 0 ?  resY : resX);
    CartesianPoint2d p;
    p.x=( c.x() * resX + originX );
    p.y=( c.y() * tmpResY + originY );
    return  p;
}

double orcapathplan::euclideanDistance( const orcaogmap::CartesianPoint2d p1, const orcaogmap::CartesianPoint2d p2 )
{
    return sqrt( pow(p1.x-p2.x,2.0) + pow(p1.y-p2.y,2.0) );
}

