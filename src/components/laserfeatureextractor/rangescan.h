/*
 *  Orca-Base: Components for robotics.
 *
 *  Copyright (C) 2004
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

/*!
  \file rangescan.h
*/

#ifndef RANGESCAN_H
#define RANGESCAN_H

#include <iostream>
#include <vector>

typedef enum {
  RANGE_BEARING_HIGH_INTENSITY,
  RANGE_BEARING_CORNER,
  RANGE_BEARING_POSSIBLE_CORNER,
  RANGE_BEARING_FOREGROUND_POINT
} RangeBearingType;

/*!
  \brief Defines a range-bearing pair in sensor (polar) coordinate system.
*/

class RangeBearing
{
public:
    //! constructor
    RangeBearing( double R=0.0, double B=0.0 ) :
        range( R ), bearing( B ) {}

    /*!
      \var double range
      Range to return
    */
    double range;

    /*!
      \var double bearing
      Bearing to return
    */
    double bearing;
    
    /*!
      \var RangeBearingType type
      Type of Range/Bearing observation
    */
    RangeBearingType type;
};

#endif
