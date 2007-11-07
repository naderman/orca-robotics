#ifndef IEXTRACTOR_H
#define IEXTRACTOR_H

namespace laserfeatures {

//!
//! @author Alex Brooks
//!
class IExtractor
{

public: 

    virtual ~IExtractor() {};

    // Adds laser features to the 'features' data structure
    virtual void addFeatures( const orca::LaserScanner2dDataPtr &laserData,
                              orca::PolarFeature2dData          &features )=0;

private: 


};

}

#endif
