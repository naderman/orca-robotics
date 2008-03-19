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

#ifndef ALGORITHM_DRIVER_H
#define ALGORITHM_DRIVER_H

#include <orca/polarfeature2d.h>
#include <orca/laserscanner2d.h>

namespace laserfeatures
{

/*

\brief Abstract class for a laser feature extractor

\author Tobias Kaupp t.kaupp at cas.edu.au, Alex Brooks

*/
class Driver 
{

public:

    virtual ~Driver() {};
    
    // Computes the features
    virtual int computeFeatures( const orca::LaserScanner2dDataPtr  &laserDataPtr,
                                       orca::PolarFeature2dData     &featureData ) = 0;
};

} // namespace

#endif
