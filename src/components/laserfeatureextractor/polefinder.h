/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp,
 *               George Mathews, Stef Williams.
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef POLE_FINDER_H
#define POLE_FINDER_H

#include <vector>

namespace orca_polefinder {

    // The pole goes from from startI to endI inclusive,
    // where startI and endI are indices into the scan
    struct Pole
    {
        Pole() {}
        Pole( int startI, int endI )
            { this->startI = startI; this->endI = endI; }
        int    startI;
        int    endI;
    };

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
    int detect_poles( const std::vector<float> &ranges,
                      double startAngle,
                      double angleIncrement,
                      double maxLaserRange,
                      double min_width,
                      double max_width,
                      double min_distance_to_background,
                      double min_angle_from_dodge,
                      std::vector<Pole> &poles );
}

#endif
