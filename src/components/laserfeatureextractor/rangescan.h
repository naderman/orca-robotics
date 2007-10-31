/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp,
 *               George Mathews, Stef Williams.
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
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

/*
  \brief Defines a range-bearing pair in sensor (polar) coordinate system.
*/

class RangeBearing
{
public:
    // constructor
    RangeBearing( double R=0.0, double B=0.0 ) :
        range( R ), bearing( B ) {}

    /*
      \var double range
      Range to return
    */
    double range;

    /*
      \var double bearing
      Bearing to return
    */
    double bearing;
    
    /*
      \var RangeBearingType type
      Type of Range/Bearing observation
    */
    RangeBearingType type;
};

#endif
