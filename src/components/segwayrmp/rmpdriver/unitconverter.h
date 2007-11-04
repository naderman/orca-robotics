#ifndef SEGWAYRMP_UNITCONVERTER_H
#define SEGWAYRMP_UNITCONVERTER_H

#include <rmpdriver/rmpdefs.h>

namespace segwayrmp {

//
// @brief convert between S.I. units and RMP native units (counts)
//
// @author Alex Brooks
//
class UnitConverter
{

public: 

    UnitConverter( RmpModel model );

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

    // S.I. units -> counts
    int speedInCounts( double si ) const;
    int angularRateInCounts( double si ) const;

private: 

    double countsPerM() const;
    double countsPerMperS() const;
    double countsPerRevolution() const;
    double countsPerNM() const;

    RmpModel model_;

};

}

#endif
