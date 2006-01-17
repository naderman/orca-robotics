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

#ifndef POLE_FINDER_H
#define POLE_FINDER_H

#include <orca/laser.h>
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

    //!
    //! \author Alex Brooks
    //!
    //! \brief Finds poles.  Returns the number of poles found.
    //!
    //! The actual locations of the poles are returned in the 'poles' array 
    //! (should already have space allocated to it).
    //! 
    //! Other parameters are:
    //!   max_laser_range (obvious)
    //!   min_width (to be declared a pole) [m]
    //!   max_width (to be declared a pole) [m]
    //!   min_distance_to_background: minimum distance of the 'pole' from the 
    //!     background [m]
    //!   min_angle_from_dodge: pole should be separated by some angle from dodgy shit like max ranges.
    //!
    //!     ie for a laser scan looking like:
    //!
    //!     -      -----  -----
    //!     |          |  |
    //!    x|          |  |       Require: x >= min_distance_to_background
    //!     |          |  |                min_width < y < max_width
    //!     -          ----                to declare this a pole.
    //!                
    //!                |--|
    //!                 y
    //!
    int detect_poles( const orca::RangeScannerConfigPtr laserConf,
                      const orca::LaserDataPtr ranges,
                      double maxLaserRange,
                      double min_width,
                      double max_width,
                      double min_distance_to_background,
                      double min_angle_from_dodge,
                      std::vector<positionRB> &poles );
}

#endif
