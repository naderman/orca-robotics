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
#include "driver.h"
#include <vector>
#include "iextractor.h"

namespace laserfeatures
{

class CombinedDriver: public Driver
{

public:

    CombinedDriver( const orca::RangeScanner2dDescription &laserDescr,
                    const orcaice::Context &context );
    virtual ~CombinedDriver();
    
    virtual int computeFeatures( const orca::LaserScanner2dDataPtr &laserDataPtr,
                                 orca::PolarFeature2dData          &featureData );
    
private:

    std::vector<IExtractor*> extractors_;
};

} // namespace

#endif
