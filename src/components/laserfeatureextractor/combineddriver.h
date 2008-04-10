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
#include <hydrolaserfeatureextract/hydrolaserfeatureextract.h>
#include <hydrolaserfeatureextract/linesegments.h>

namespace laserfeatures
{

class IExtractor
{
public: 

    virtual ~IExtractor() {}

    // Adds laser features to the 'features' data structure
    virtual void addFeatures( const orca::LaserScanner2dDataPtr &laserData,
                              orca::PolarFeature2dData          &features )=0;

private: 

};

class CombinedDriver: public Driver
{

public:

    CombinedDriver( const orca::RangeScanner2dDescription &laserDescr,
                    const orcaice::Context &context );
    
    virtual int computeFeatures( const orca::LaserScanner2dDataPtr &laserDataPtr,
                                 orca::PolarFeature2dData          &featureData );
    
private:

    hydrolfextract::StartOrEnd s;

    std::auto_ptr<hydrolfextract::ReflectorFeatureExtractor>     reflectorExtractor_;
    std::auto_ptr<hydrolfextract::ForegroundFeatureExtractor>    foregroundExtractor_;
    std::auto_ptr<hydrolfextract::DoorFeatureExtractor>          doorExtractor_;
    std::auto_ptr<hydrolfextract::LineAndCornerFeatureExtractor> lineAndCornerExtractor_;
};

} // namespace

#endif
