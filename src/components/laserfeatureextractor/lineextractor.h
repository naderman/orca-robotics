/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp,
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
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

    LineExtractor( const orcaice::Context &context,
                   double laserMaxRange,
                   bool extractLines,
                   bool extractCorners );

    // Adds laser features to the 'features' data structure
    void addFeatures( const orca::LaserScanner2dDataPtr &laserData,
                      orca::PolarFeature2dData &features );  
    
    ////////////////////////////////////////

    enum VisibilityReason {
        InternalLineJunction,
        ExternalLineJunction,
        Unknown
    };

private: 
    
    double laserMaxRange_;

    bool extractLines_;
    bool extractCorners_;

    // config
    double clusterMaxRangeDelta_;
    double breakDistThreshold_;
    int    minPointsInLine_;
    double minLineLength_;
    
    double rangeSd_;
    double bearingSd_;

    // reject observations of lines with near-zero slope
    bool rejectLikelyGroundObservations_;

    double linePFalsePositive_;
    double linePFalsePositivePossibleGround_;
    double linePTruePositive_;

    double cornerPFalsePositive_;
    double cornerPTruePositive_;

    orcaice::Context context_;

    void addLines( const std::vector<Section> &sections, 
                   orca::PolarFeature2dData   &features );
    void addCorners( const std::vector<Section> &sections, 
                     orca::PolarFeature2dData   &features,
                     double angleIncrement );

    bool isStartVisible( const Section &section,
                         double alpha,
                         const Section *prevSection,
                         VisibilityReason *reason=NULL,
                         double minLineLength=-1 );
    bool isEndVisible( const Section &section,
                       double alpha,
                       const Section *nextSection,
                       VisibilityReason *reason=NULL,
                       double minLineLength=-1 );

    void determineUncertainty( double &rhoSd, double &alphaSd, const Section &s );

};

}

#endif
