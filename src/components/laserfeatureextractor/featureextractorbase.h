#ifndef ORCA2_FEATURE_EXTRACTOR_BASE_H
#define ORCA2_FEATURE_EXTRACTOR_BASE_H

// #include <orca/laser.h>

#include "configparameters.h"

/*!

\brief Abstract class for a laser feature extractor

\author Tobias Kaupp t.kaupp at cas.edu.au

*/
class FeatureExtractorBase 
{

public:

    FeatureExtractorBase();
    virtual ~FeatureExtractorBase();
    virtual void initialize( ConfigParameters *configParameters) = 0;

private:

};

#endif
