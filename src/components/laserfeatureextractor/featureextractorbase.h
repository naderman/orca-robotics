#ifndef ORCA2_FEATURE_EXTRACTOR_BASE_H
#define ORCA2_FEATURE_EXTRACTOR_BASE_H

#include "configparameters.h"

/*!

\brief Abstract class for a laser feature extractor

\author Tobias Kaupp t.kaupp at cas.edu.au

*/

#include <orca/polarfeature2d.h>
#include <orca/laser.h>

class FeatureExtractorBase 
{

public:

    FeatureExtractorBase();
    virtual ~FeatureExtractorBase();
    
    //! Passes configuration parameters to algorithm which initializes itself
    virtual int initialize( ConfigParameters *configParameters) = 0;
    
    //! Computes the features
    virtual int computeFeatures( const orca::LaserConfigPtr laserConfigPtr, const orca::LaserDataPtr laserDataPtr, orca::PolarFeature2dDataPtr featureDataPtr) = 0;

private:

};

#endif
