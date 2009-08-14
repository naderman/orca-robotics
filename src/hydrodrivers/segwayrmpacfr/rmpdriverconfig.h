/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_SEGWAY_RMP_DRIVER_CONFIG_H
#define ORCA2_SEGWAY_RMP_DRIVER_CONFIG_H

#include <string>
#include <stdint.h>
#include <hydroutil/context.h>
#include "rmpdefs.h"

namespace segwayrmpacfr
{

//
// Convenience structure to hold all the config parameters.
// All units are S.I.
//
class RmpDriverConfig 
{
public:

    RmpDriverConfig();

    GainSchedule gainSchedule;
    double maxVelocityScale;
    double maxTurnrateScale;
    double maxAccelerationScale;
    double maxCurrentLimitScale;

    RmpModel model;
    bool     requireSpecificBuildId;
    // The required RMP firmware version (only valid if requireSpecificBuildId is set)
    int16_t  requiredBuildId;

    bool allowMoveInTractorMode;
    bool allowMoveInBalanceMode;

    // for debugging
    bool logCanDataToFile;
    std::string canDataLogFileName;

    // returns 0 if everything looks good.
    int checkSanity( std::string &errors );
};

std::ostream &operator<<( std::ostream & s, const RmpDriverConfig & c );

void readFromProperties( const hydroutil::Context & context, RmpDriverConfig & c );

} // namespace

#endif
