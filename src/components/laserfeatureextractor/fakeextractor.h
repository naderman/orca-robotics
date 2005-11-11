#ifndef ORCA2_FAKE_EXTRACTOR_H
#define ORCA2_FAKE_EXTRACTOR_H

#include "featureextractorbase.h"
#include "configparameters.h"

/*!

\brief

\author Tobias Kaupp t.kaupp at cas.edu.au

*/
class FakeExtractor: public FeatureExtractorBase 
{

public:

    FakeExtractor();
    virtual ~FakeExtractor();
    
    int initialize( ConfigParameters *configParameters );
    int computeFeatures( const orca::RangeScannerConfigPtr laserConfigPtr, const orca::LaserDataPtr laserDataPtr, orca::PolarFeature2dDataPtr featureDataPtr);
    
private:

};

#endif
