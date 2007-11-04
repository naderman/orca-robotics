/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_SEGWAY_RMP_DRIVER_CONFIG_H
#define ORCA2_SEGWAY_RMP_DRIVER_CONFIG_H

#include <string>
#include <orcaice/context.h>
#include <rmpdriver/rmpdefs.h>

namespace segwayrmp
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

    // returns 0 if everything looks good.
    int checkSanity( std::string &errors );
};

std::ostream &operator<<( std::ostream & s, const RmpDriverConfig & c );

void readFromProperties( const orcaice::Context & context, RmpDriverConfig & c );

} // namespace

#endif
