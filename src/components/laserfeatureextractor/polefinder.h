/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp,
 *               George Mathews, Stef Williams.
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef POLE_FINDER_H
#define POLE_FINDER_H

#include <orca/laserscanner2d.h>
#include <vector>

namespace orca_polefinder {

// Suggested values
// #define MIN_LEG_WIDTH      0.05 // [m]
// #define MAX_LEG_WIDTH      0.2  // [m]
// #define MIN_LEG_DIST_FROM_BACKGROUND 0.3 // [m]
// #define MAX_LEG_SEPARATION 0.5 // [m]

    typedef struct
    {
        double range;   // [m]
        double bearing; // [rad]
    } positionRB;

    //
    // \author Alex Brooks
    //
    // \brief Finds poles.  Returns the number of poles found.
    //
    // The actual locations of the poles are returned in the 'poles' array
    // (should already have space allocated to it).
    //
    // Other parameters are:
    //   max_laser_range (obvious)
    //   min_width (to be declared a pole) [m]
    //   max_width (to be declared a pole) [m]
    //   min_distance_to_background: minimum distance of the 'pole' from the
    //     background [m]
    //   min_angle_from_dodge: pole should be separated by some angle from dodgy shit like max ranges.
    //
    //     ie for a laser scan looking like:
    //
    //     -      -----  -----
    //     |          |  |
    //    x|          |  |       Require: x >= min_distance_to_background
    //     |          |  |                min_width < y < max_width
    //     -          ----                to declare this a pole.
    //
    //                |--|
    //                 y
    //
    int detect_poles( const orca::RangeScanner2dConfigPtr laserConf,
                      const orca::LaserScanner2dDataPtr ranges,
                      double maxLaserRange,
                      double min_width,
                      double max_width,
                      double min_distance_to_background,
                      double min_angle_from_dodge,
                      std::vector<positionRB> &poles );
}

#endif
