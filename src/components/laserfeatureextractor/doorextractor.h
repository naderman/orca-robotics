#ifndef DOOREXTRACTOR__H
#define DOOREXTRACTOR__H

#include <orca/laser.h>
#include <orca/polarfeature2d.h>


namespace laserfeatures {

class DoorExtractor
{

public: 

    DoorExtractor() {}

//     Adds laser features to the 'features' data structure
      void addFeatures( const orca::LaserDataPtr &laserData,
                        orca::PolarFeature2dDataPtr &features );   

private: 
        
};

}

#endif
