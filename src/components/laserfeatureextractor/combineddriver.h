/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp,
 *               George Mathews, Stef Williams.
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef COMBINED_DRIVER_H
#define COMBINED_DRIVER_H

#include <orcaice/context.h>
#include "algorithmdriver.h"
#include <vector>
#include "iextractor.h"

namespace laserfeatures
{

class CombinedDriver: public AlgorithmDriver
{

public:

    CombinedDriver( const orcaice::Context & context, double maxRange, int numberOfSamples );
    virtual ~CombinedDriver();
    
    virtual int computeFeatures( const orca::LaserScanner2dDataPtr &laserDataPtr,
                                 orca::PolarFeature2dData          &featureData );
    
private:

    std::vector<IExtractor*> extractors_;
};

} // namespace

#endif
