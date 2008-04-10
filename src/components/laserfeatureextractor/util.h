#ifndef LASERFEATUREEXTRATOR_UTIL_H
#define LASERFEATUREEXTRATOR_UTIL_H

#include <orca/rangescanner2d.h>
#include <orca/polarfeature2d.h>
#include <orcaobj/miscutils.h>
#include <hydrolaserfeatureextract/scanutil.h>
#include <hydrofeatureobs/features.h>

namespace laserfeatures {

inline hydrolfextract::ScannerConfig getScannerConfig( const orca::RangeScanner2dDescription &laserDescr )
{
    return hydrolfextract::ScannerConfig( laserDescr.maxRange,
                                        laserDescr.startAngle,
                                        orcaobj::calcAngleIncrement( laserDescr.fieldOfView, 
                                                                     laserDescr.numberOfSamples ) );
}

inline double laserScanBearing( const orca::RangeScanner2dDataPtr &scan, const int i )
{
    double angleIncrement = orcaobj::calcAngleIncrement( scan->fieldOfView, scan->ranges.size() );
    return (scan->startAngle + angleIncrement*i);
}

orca::SinglePolarFeature2dPtr convert( const hydrofeatureobs::FeatureObs &hydroFeature );

}

#endif
