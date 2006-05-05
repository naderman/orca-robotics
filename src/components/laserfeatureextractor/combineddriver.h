/*
 *  Orca Project: Components for robotics.
 *
 *  Copyright (C) 2004-2006
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
 
#ifndef ORCA2_LASERFEATUREEXTRACTOR_COMBINED_DRIVER_H
#define ORCA2_LASERFEATUREEXTRACTOR_COMBINED_DRIVER_H

#include "algorithmdriver.h"

class ReflectorExtractor;
class ForegroundExtractor;
class DoorExtractor;
class CornerExtractor;

namespace laserfeatures
{

class CombinedDriver: public AlgorithmDriver
{

public:

    struct Config
    {
        // which algs
        int    extractReflectors; 
        int    extractForegroundPoints; 
        int    extractCorners; 
        int    extractDoors; 
        // reflector params
        double maxDeltaRangeNearReflector;
        double maxDeltaRangeWithinReflector;
        int    minReflectorBrightness; 
        // foreground params
        double minForegroundWidth;
        double maxForegroundWidth;
        double minForegroundBackgroundSeparation;
    };
    
    CombinedDriver( const Config & config );
    virtual ~CombinedDriver();
    
    virtual int computeFeatures( const orca::RangeScannerConfigPtr &laserConfigPtr,
                                 const orca::LaserDataPtr          &laserDataPtr,
                                 orca::PolarFeature2dDataPtr       &featureDataPtr );
    
    virtual void setMaxRange( float maxRange );

private:

    ReflectorExtractor *reflectorExtractor_;
    ForegroundExtractor *foregroundExtractor_;
    DoorExtractor *doorExtractor_;
    CornerExtractor *cornerExtractor_;

    Config config_;

};

} // namespace

#endif
