/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef VFHALGORITHMCONFIG_H
#define VFHALGORITHMCONFIG_H

#include <string>
#include <orcaice/context.h>

namespace vfh {

//
// Convenience structure to hold all the vfh_algorithm parameters.
// All units are S.I.
//
class VfhAlgorithmConfig 
{
public:

    VfhAlgorithmConfig();

    // VFH's world is square, with sides of length (cell_size * worldWidthInCells),
    double cellSize;
    int    gridWidthInCells;
    // VFH's polar histogram is split up into sectors
    double sectorAngle;
    // Radius of the assumed-circular robot
    double robotRadius;
    // safety_dist depends linearly on speed
    double safetyDist0ms;
    double safetyDist1ms; 
    // Absolute max speed
    double maxSpeed;
    // Max speed through a narrow opening
    double maxSpeedNarrowOpening;
    // Max speed through a wide opening
    double maxSpeedWideOpening;
    double maxAcceleration;
    // Max turnrate is linearly related to speed
    double maxTurnrate0ms;
    double maxTurnrate1ms;
    // For safety, this sets a hard maximum.
    double absoluteMaxTurnrate;
    // Increase min_turning_radius a bit to handle side-slip
    double minTurnRadiusSafetyFactor;
    // free_space_cutoff is 'Binary_Hist_Low', and
    // obs_cutoff        is 'Binary_Hist_High'.
    //
    // These are the thresholds that determine whether sectors are free or empty.
    // There are two because hysteresis thresholding is used.
    // The units are unclear.
    //
    // These also depend linearly on speed.
    double freeSpaceCutoff0ms;
    double obsCutoff0ms;
    double freeSpaceCutoff1ms;
    double obsCutoff1ms;
    // VFH has a bit of hysteresis in its actions that forces it to commit
    double weightDesiredDir;
    double weightCurrentDir;

    // returns 0 if everything looks good.
    int checkSanity( std::string &warnings, std::string &errors );
};

std::ostream &operator<<( std::ostream &s, const VfhAlgorithmConfig &c );

void readFromProperties( orcaice::Context context, VfhAlgorithmConfig &c );

}

#endif
