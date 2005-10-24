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
    
    void initialize( ConfigParameters *configParameters ) {};
    
private:

};

#endif
