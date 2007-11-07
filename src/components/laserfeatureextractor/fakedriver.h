/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp,
 *               George Mathews, Stef Williams.
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA2_LASERFEATUREEXTRACTOR_FAKE_DRIVER_H
#define ORCA2_LASERFEATUREEXTRACTOR_FAKE_DRIVER_H

#include "algorithmdriver.h"

namespace laserfeatures
{

class FakeDriver: public AlgorithmDriver
{

public:

    FakeDriver();
    
    virtual int computeFeatures( const orca::LaserScanner2dDataPtr &laserDataPtr,
                                 orca::PolarFeature2dData          &featureData );
};

} // namespace

#endif
