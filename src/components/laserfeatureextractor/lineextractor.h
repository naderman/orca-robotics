/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp,
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef LINEEXTRACTOR__H
#define LINEEXTRACTOR__H

#include <orca/laserscanner2d.h>
#include <orca/polarfeature2d.h>
#include <vector>
#include "section.h"
#include <orcaice/context.h>
#include "iextractor.h"

namespace laserfeatures {

class LineExtractor : public IExtractor
{

public: 

    LineExtractor( orcaice::Context, double laserMaxRange, bool extractLines, bool extractCorners );

    // Adds laser features to the 'features' data structure
    void addFeatures( const orca::LaserScanner2dDataPtr &laserData,
                      orca::PolarFeature2dDataPtr &features );  
    
private: 
    
    double laserMaxRange_;

    bool extractLines_;
    bool extractCorners_;

    // config
    double clusterMaxRangeDelta_;
    double breakDistThreshold_;
    int    minPointsInLine_;
    double minLineLength_;
    
    double rhoSd_;
    double alphaSd_;

    // Not used at the moment
    // bool extractPossibleCorners( const orca::PolarFeature2dDataPtr & featureDataPtr );
    
    void addCorners( const std::vector<Section> &sections, 
                     orca::PolarFeature2dDataPtr &features );
    void addLines( const std::vector<Section> &sections, 
                   orca::PolarFeature2dDataPtr &features );

    bool isStartVisible( const Section &section,
                         double alpha,
                         const Section *prevSection );
    bool isEndVisible( const Section &section,
                       double alpha,
                       const Section *nextSection );
};

}

#endif
