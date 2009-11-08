/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "unitconverter.h"
#include <iostream>
#include <cmath>
#include <assert.h>
#include <gbxutilacfr/exceptions.h>
#include <sstream>

using namespace std;

namespace segwayrmpacfr {

UnitConverter::UnitConverter( RmpModel model )
    : model_(model)
{
    if ( ! ( model_ >= RmpModel_50 &&
             model_ <= RmpModel_400 ) )
    {
        stringstream ss;
        ss << "UnitConverter: unknown RmpModel: " << model_;
        throw gbxutilacfr::Exception( ERROR_INFO, ss.str() );
    }
}

double 
UnitConverter::angleInRadiansFromRevCounts( int counts ) const
{
    return ((double)counts/countsPerRevolution())*2.0*M_PI;
}
double 
UnitConverter::angleInRadians( int counts ) const
{
    return (double)counts/RMP_COUNT_PER_RAD;
}
double 
UnitConverter::angularRateInRadians( int counts ) const
{
    return (double)counts/RMP_COUNT_PER_RAD_PER_S;
}
double 
UnitConverter::distanceInM( int counts ) const
{
    return (double)counts/countsPerM();
}
double 
UnitConverter::speedInMperS( int counts ) const
{
    return (double)counts/countsPerMperS();
}
double
UnitConverter::torqueInNM( int counts ) const
{
    return (double)counts/countsPerNM();
}
double
UnitConverter::uiVoltageInVolts( int counts ) const
{
    return RMP_UI_OFFSET + (double)counts*RMP_UI_COEFF;
}
double
UnitConverter::baseVoltageInVolts( int counts ) const
{
    return (double)counts/RMP_BASE_COUNT_PER_VOLT;
}


int
UnitConverter::speedCommandAsRaw( double si ) const
{
    return (int)(round(si*countsPerMperS()));
}
int
UnitConverter::angularRateCommandAsRaw( double si ) const
{
    return (int)(round(si*RMP_COUNT_PER_RAD_PER_S_CMD));
}
OperationalMode
UnitConverter::operationalModeAsEnum( int i ) const
{
    // Cast is OK, the enum values are lined up correctly.
    return (OperationalMode) i;
}
GainSchedule
UnitConverter::gainScheduleAsEnum( int i ) const
{
    // Cast is OK, the enum values are lined up correctly.
    return (GainSchedule) i;
}
uint16_t
UnitConverter::configurationCommandAsRaw( ConfigurationCommand cmd ) const
{
    // Cast is OK, the enum values are lined up correctly.
    return (uint16_t) cmd;
}
uint16_t
UnitConverter::maxVelocityScaleFactorAsRaw( double scale ) const
{
    return (uint16_t)ceil(scale*16.0);
}
uint16_t
UnitConverter::maxTurnrateScaleFactorAsRaw( double scale ) const
{
    return (uint16_t)ceil(scale*16.0);
}
uint16_t
UnitConverter::maxAccelerationScaleFactorAsRaw( double scale ) const
{
    return (uint16_t)ceil(scale*16.0);
}
uint16_t
UnitConverter::maxCurrentLimitScaleFactorAsRaw( double scale ) const
{
    // note: the range of this command is [0,256]
    return (uint16_t)ceil(scale*256.0);
}

double 
UnitConverter::countsPerM() const
{
    switch ( model_ )
    {
    case RmpModel_50:
        return RMP_COUNT_PER_M_RMP50;
    case RmpModel_100:
        return RMP_COUNT_PER_M_RMP100;
    case RmpModel_200:
        return RMP_COUNT_PER_M_RMP200;
    case RmpModel_400:
        return RMP_COUNT_PER_M_RMP400;
    default:
        throw gbxutilacfr::Exception( ERROR_INFO, "UnitConverter::countsPerM(): unknown RmpModel" );
    }
}
double 
UnitConverter::countsPerMperS() const
{
    switch ( model_ )
    {
    case RmpModel_50:
        return RMP_COUNT_PER_M_PER_S_RMP50;
    case RmpModel_100:
        return RMP_COUNT_PER_M_PER_S_RMP100;
    case RmpModel_200:
        return RMP_COUNT_PER_M_PER_S_RMP200;
    case RmpModel_400:
        return RMP_COUNT_PER_M_PER_S_RMP400;
    default:
        throw gbxutilacfr::Exception( ERROR_INFO, "UnitConverter::countsPerMperS(): unknown RmpModel" );
    }
}

double
UnitConverter::countsPerRevolution() const
{
    switch ( model_ )
    {
    case RmpModel_50:
        return RMP_COUNT_PER_REV_RMP50;
    case RmpModel_100:
        return RMP_COUNT_PER_REV_RMP100;
    case RmpModel_200:
        return RMP_COUNT_PER_REV_RMP200;
    case RmpModel_400:
        return RMP_COUNT_PER_REV_RMP400;
    default:
        throw gbxutilacfr::Exception( ERROR_INFO, "UnitConverter::countsPerRevolution(): unknown RmpModel" );
    }    
}
double
UnitConverter::countsPerNM() const
{
    switch ( model_ )
    {
    case RmpModel_50:
        return RMP_COUNT_PER_NM_RMP50;
    case RmpModel_100:
        return RMP_COUNT_PER_NM_RMP100;
    case RmpModel_200:
        return RMP_COUNT_PER_NM_RMP200;
    case RmpModel_400:
        return RMP_COUNT_PER_NM_RMP400;
    default:
        throw gbxutilacfr::Exception( ERROR_INFO, "UnitConverter::countsPerNM(): unknown RmpModel" );
    }    
}

}
