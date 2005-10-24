#ifndef ORCA2_EXTRACTOR_ONE_H
#define ORCA2_EXTRACTOR_ONE_H

#include "featureextractorbase.h"
#include "configparameters.h"

/*!

\brief

\author Tobias Kaupp t.kaupp at cas.edu.au

*/
class ExtractorOne: public FeatureExtractorBase 
{

public:

    ExtractorOne();
    virtual ~ExtractorOne();
    
    void initialize( ConfigParameters *configParameters ) {};
    
private:

};

#endif
