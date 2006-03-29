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

#include <vector>
#include <list>

#include "algorithmdriver.h"
#include "reflectorextractor.h"
#include "section.h"

namespace laserfeatures
{

/*
    \brief
    \author Tobias Kaupp t.kaupp at cas.edu.au
*/
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
    
    virtual void setMaxRange( float maxRange )
        { 
            reflectorExtractor_.setMaxRange( maxRange );
            maxLaserRange_ = maxRange; 
        }

private:

    ReflectorExtractor reflectorExtractor_;

    double maxLaserRange_;

    std::vector<Section> sections_;

    Config config_;
    
    // uitility functions for each type of feature extraction
//     bool extractLaserReflectors( const orca::LaserDataPtr & laserDataPtr,
//                                  const orca::PolarFeature2dDataPtr & featureDataPtr);
    bool extractForegroundPoints( const orca::RangeScannerConfigPtr & laserConfigPtr,
                                  const orca::LaserDataPtr & laserDataPtr,
                                  const orca::PolarFeature2dDataPtr & featureDataPtr);
    bool extractDoors( const orca::LaserDataPtr & laserDataPtr,
                       const orca::PolarFeature2dDataPtr & featureDataPtr);
    bool extractCorners( const orca::LaserDataPtr & laserDataPtr,
                         const orca::PolarFeature2dDataPtr & featureDataPtr);
    bool extractPossibleCorners( const orca::PolarFeature2dDataPtr & featureDataPtr );
    
    //
    // Corner detector routines
    //void calculatePos(const orca::LaserDataPtr laserDataPtr);
    void connectSections(const orca::LaserDataPtr & laserDataPtr);
    void extractLines();
    void findBreakPoint(Section &s, double &maxDist, int &pos);
    void fitLine(Section &s);
    void printSections();

};

} // namespace

#endif
