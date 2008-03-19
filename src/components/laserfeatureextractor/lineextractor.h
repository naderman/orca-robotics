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
#include <orcaice/context.h>
#include "iextractor.h"
#include <hydrolaserfeatureextract/hydrolaserfeatureextract.h>

namespace laserfeatures {

class LineExtractor : public IExtractor
{

public: 

    LineExtractor( const orca::RangeScanner2dDescription &laserDescr,
                   bool extractLines,
                   bool extractCorners,
                   const orcaice::Context &context );

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

    std::auto_ptr<hydrolfextract::LineExtractor> lineExtractor_;
    
};

}

#endif
