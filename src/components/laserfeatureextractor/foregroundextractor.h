#ifndef FOREGROUNDEXTRACTOR__H
#define FOREGROUNDEXTRACTOR__H

#include <orca/laser.h>
#include <orca/polarfeature2d.h>


namespace laserfeatures {

class ForegroundExtractor
{

public: 

    ForegroundExtractor( double minForegroundWidth,
                         double maxForegroundWidth,
                         double minForegroundBackgroundSeparation )
    : minForegroundWidth_(minForegroundWidth),
      maxForegroundWidth_(maxForegroundWidth),
      minForegroundBackgroundSeparation_(minForegroundBackgroundSeparation),
      laserMaxRange_( -1 )
      {}

//     Adds laser features to the 'features' data structure
      void addFeatures( const orca::LaserDataPtr &laserData,
                        const orca::RangeScannerConfigPtr &laserConfig,
                        orca::PolarFeature2dDataPtr &features );   


    void setMaxRange( double maxRange ) { laserMaxRange_ = maxRange; }

private: 
    
    double minForegroundWidth_;
    double maxForegroundWidth_;
    double minForegroundBackgroundSeparation_;
    double laserMaxRange_;
    
};

}

#endif
