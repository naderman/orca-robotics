#include "unitconverter.h"
#include <iostream>
#include <cmath>
#include <assert.h>
#include <rmpexception.h>
#include <sstream>

using namespace std;

namespace segwayrmp {

UnitConverter::UnitConverter( RmpModel model )
    : model_(model)
{
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
UnitConverter::speedInCounts( double si ) const
{
    return si*countsPerMperS();
}
int
UnitConverter::angularRateInCounts( double si ) const
{
    return si*RMP_COUNT_PER_RAD_PER_S;
}
OperationalMode
UnitConverter::operationalModeAsEnum( int i ) const
{
    switch ( i )
    {
    case 0:
        return OperationalModeDisabled;
    case 1:
        return OperationalModeTractor;
    case 2:
        return OperationalModeBalance;
    default:
        stringstream ss;
        ss << "UnitConverter: unknown OperationalMode: " << i;
        throw ss.str();
    }
}
GainSchedule
UnitConverter::gainScheduleAsEnum( int i ) const
{
    switch ( i )
    {
    case 0:
        return GainScheduleNormal;
    case 1:
        return GainScheduleTall;
    case 2:
        return GainScheduleHeavy;
    default:
        stringstream ss;
        ss << "UnitConverter: unknown GainSchedule: " << i;
        throw ss.str();
    }
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
        throw RmpException( "unknown RmpModel" );
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
        throw RmpException( "unknown RmpModel" );
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
        throw RmpException( "unknown RmpModel" );
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
        throw RmpException( "unknown RmpModel" );
    }    
}

}
