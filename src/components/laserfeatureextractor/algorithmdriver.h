/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp,
 *               George Mathews, Stef Williams.
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_LASERFEATUREEXTRACTOR_ALGORITHM_DRIVER_H
#define ORCA2_LASERFEATUREEXTRACTOR_ALGORITHM_DRIVER_H

#include <orca/polarfeature2d.h>
#include <orca/laser.h>

namespace laserfeatures
{

/*

\brief Abstract class for a laser feature extractor

\author Tobias Kaupp t.kaupp at cas.edu.au

*/
class AlgorithmDriver 
{

public:

    virtual ~AlgorithmDriver() {};
    
    // Computes the features
    virtual int computeFeatures( const orca::RangeScannerConfigPtr &laserConfigPtr,
                                 const orca::LaserDataPtr          &laserDataPtr,
                                       orca::PolarFeature2dDataPtr &featureDataPtr ) = 0;

    virtual void setMaxRange( float maxRange )=0;
                                 
};

} // namespace

#endif
