/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef POLE_FINDER_H
#define POLE_FINDER_H

#include <vector>
#include <string>
#include <hydroscanutil/scanutil.h>

namespace hydrolfextract {

    // The pole goes from from startI to endI inclusive,
    // where startI and endI are indices into the scan
    struct Pole
    {
        Pole() {}
        Pole( int startI, int endI, double range, double bearing, double width )
            { 
                this->startI = startI; this->endI = endI; 
                this->range=range; this->bearing=bearing; 
                this->width=width; 
            }
        int    startI;
        int    endI;
        double range;
        double bearing;
        double width;
    };
    std::string toString( const Pole &p );

    //
    // \author Alex Brooks
    //
    // \brief Finds poles.
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
    std::vector<Pole> detectPoles( const std::vector<float> &ranges,
                                   const hydroscanutil::ScannerConfig &scannerCfg,
                                   double min_width,
                                   double max_width,
                                   double min_distance_to_background,
                                   bool debug=false );
}

#endif
