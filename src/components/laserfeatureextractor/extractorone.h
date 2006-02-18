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
#ifndef ORCA2_EXTRACTOR_ONE_H
#define ORCA2_EXTRACTOR_ONE_H

#include <vector>
#include <list>

#include "section.h"
#include "featureextractorbase.h"
#include "configparameters.h"

// Stage values....Real Values
#define MAX_RANGE    79.99     // 7.99    79.99
#define NO_SECTION    -1
#define RANGE_DELTA     1.0    // 0.10    0.2
#define BREAK_DIST     0.5    // 0.4    0.2
#define MIN_POINTS_IN_LINE   6    // 6      6
#define ERROR_THRESHOLD   0.24    // 0.10    0.24
#define CORNER_BOUND     0.45    // 0.45    0.45
#define POSSIBLE_BOUND    0.2    // ???    0.2

/*

\brief

\author Tobias Kaupp t.kaupp at cas.edu.au

*/
class ExtractorOne: public FeatureExtractorBase 
{

public:

    ExtractorOne();
    virtual ~ExtractorOne();
    
    int initialize( ConfigParameters *configParameters );
    int computeFeatures( const orca::RangeScannerConfigPtr laserConfigPtr, const orca::LaserDataPtr laserDataPtr, orca::PolarFeature2dDataPtr featureDataPtr);
    
private:
    
    int extractReflectors_;
    int extractForegroundPoints_;
    int extractCorners_;
    int extractDoors_;

    // Parameters for laser reflectors
    double backgroundRangeGate_;
    double featureRangeGate_;
    int minReturns_;
    int minBrightness_;

    // Parameters for foreground points
    double minForegroundWidth_;
    double maxForegroundWidth_;
    double minForegroundBackgroundSeparation_;
    
    double laserRange_;
    
    std::vector<Section> sections_;
  
    bool extractLaserReflectors(const orca::LaserDataPtr laserDataPtr, orca::PolarFeature2dDataPtr featureDataPtr);
    bool extractForegroundPoints(const orca::RangeScannerConfigPtr laserConfigPtr, const orca::LaserDataPtr laserDataPtr, orca::PolarFeature2dDataPtr featureDataPtr);
    bool extractDoors(const orca::LaserDataPtr laserDataPtr, orca::PolarFeature2dDataPtr featureDataPtr);
    bool extractCorners(const orca::LaserDataPtr laserDataPtr, orca::PolarFeature2dDataPtr featureDataPtr);
    bool extractPossibleCorners(orca::PolarFeature2dDataPtr featureDataPtr);

    //
    // Corner detector routines
    //void calculatePos(const orca::LaserDataPtr laserDataPtr);
    void connectSections(const orca::LaserDataPtr laserDataPtr);
    void extractLines();
    void findBreakPoint(Section &s, double &maxDist, int &pos);
    void fitLine(Section &s);
    void printSections();

};

#endif
