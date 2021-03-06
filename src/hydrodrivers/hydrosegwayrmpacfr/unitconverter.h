/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef SEGWAYRMP_UNITCONVERTER_H
#define SEGWAYRMP_UNITCONVERTER_H

#include "rmpdefs.h"
#include <stdint.h>

namespace segwayrmpacfr {

//
// @brief convert between S.I. units and RMP native units (counts)
//
// @author Alex Brooks
//
class UnitConverter
{

public: 

    explicit UnitConverter( RmpModel model );

    // counts -> S.I. units
    // (some angles are specified in revs)
    double angleInRadiansFromRevCounts( int counts ) const;
    // (others are specified in degrees)
    double angleInRadians( int counts ) const;
    double angularRateInRadians( int counts ) const;
    double distanceInM( int counts ) const;
    double speedInMperS( int counts ) const;
    double torqueInNM( int counts ) const;
    double uiVoltageInVolts( int counts ) const;
    double baseVoltageInVolts( int counts ) const;
    OperationalMode operationalModeAsEnum( int i ) const;
    GainSchedule gainScheduleAsEnum( int i ) const;

    // S.I. units -> raw RMP values
    int speedCommandAsRaw( double si ) const;
    int angularRateCommandAsRaw( double si ) const;
    uint16_t configurationCommandAsRaw( ConfigurationCommand cmd ) const;
    uint16_t maxVelocityScaleFactorAsRaw( double scale ) const;
    uint16_t maxTurnrateScaleFactorAsRaw( double scale ) const;
    uint16_t maxAccelerationScaleFactorAsRaw( double scale ) const;
    uint16_t maxCurrentLimitScaleFactorAsRaw( double scale ) const;

private: 

    double countsPerM() const;
    double countsPerMperS() const;
    double countsPerRevolution() const;
    double countsPerNM() const;

    RmpModel model_;

};

}

#endif
