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

#include <orcaice/objutils.h>
#include <iostream>
#include <orcaice/mathdefs.h>

// For definitions of feature types:
#include <orca/featuremap2d.h>

#include "combineddriver.h"
#include "svd.h"
#include "polefinder.h"


// Stage values....Real Values
#define MAX_RANGE    7.99     // 7.99    79.99
#define NO_SECTION    -1
#define RANGE_DELTA     1.0    // 0.10    0.2
#define BREAK_DIST     0.5    // 0.4    0.2
#define MIN_POINTS_IN_LINE   6    // 6      6
#define ERROR_THRESHOLD   0.24    // 0.10    0.24
#define CORNER_BOUND     0.45    // 0.45    0.45
#define POSSIBLE_BOUND    0.2    // ???    0.2

using namespace std;
using namespace orcaice;
using namespace laserfeatures;

CombinedDriver::CombinedDriver( const Config & config )
    : AlgorithmDriver( config )
{
    laserRange_ = MAX_RANGE;
}

CombinedDriver::~CombinedDriver()
{
}

int
CombinedDriver::computeFeatures( const orca::RangeScannerConfigPtr & laserConfigPtr,
                                 const orca::LaserDataPtr & laserDataPtr,
                                 const orca::PolarFeature2dDataPtr & featureDataPtr )
{
    featureDataPtr->features.clear();

    if ( config_.extractReflectors ) {
        extractLaserReflectors(laserDataPtr, featureDataPtr);
    }
    
    if ( config_.extractForegroundPoints ) {
        extractForegroundPoints(laserConfigPtr, laserDataPtr, featureDataPtr);
    }
    
    if ( config_.extractDoors ) {
        extractDoors(laserDataPtr, featureDataPtr);
    }
    
    if ( config_.extractCorners ) {
        extractCorners(laserDataPtr, featureDataPtr);
    }
  
    return 0;
}


bool
CombinedDriver::extractLaserReflectors( const orca::LaserDataPtr & laserDataPtr,
                                        const orca::PolarFeature2dDataPtr & featureDataPtr)
{
    bool buildingTarget = false;

    int featureNumPnts = 0;
    double featureRange = -1.0;
    double featureBearing = -1.0;
    //int featureIntensity = -1;

    // for each return
    for ( uint i=1; i<laserDataPtr->ranges.size()-1; i++ )
    {
        //if ( laserDataPtr.intensity(i) >= config_.minBrightness && fabs( laserDataPtr.range(i-1) - laserDataPtr.range(i) ) < config_.backgroundRangeGate )
        // George:
        // include laser range and check on both sides of the return for 'possible dodgy' range
        // for a range to be doggy it usually has to have both both neighbouring ranges far away. 
        if ( laserDataPtr->intensities[i] >= config_.minBrightness &&
             laserDataPtr->ranges[i] < laserRange_ &&
             ( fabs( laserDataPtr->ranges[i-1] - laserDataPtr->ranges[i] ) < config_.backgroundRangeGate ||
               fabs( laserDataPtr->ranges[i+1] - laserDataPtr->ranges[i] ) < config_.backgroundRangeGate  ) )
        {
            // start building a new cluster of high intensity returns
            if ( buildingTarget == false )
            {
                buildingTarget = true;
                featureNumPnts = 1;
                featureRange = laserDataPtr->ranges[i];
                featureBearing = M_PI*i/(laserDataPtr->ranges.size()-1) - M_PI/2;
                continue; // go to the next return
            }

            // if the new return is close enough to the current average include it in the estimate
            if ( fabs(laserDataPtr->ranges[i] - featureRange/featureNumPnts) < config_.targetRangeGate )
            {
                ++featureNumPnts;

                featureRange += laserDataPtr->ranges[i];
                featureBearing += M_PI*i/(laserDataPtr->ranges.size()-1) - M_PI/2;
            }
        }
        else
        {
            // if we have reached the end of a cluster, process a new observation
            if ( buildingTarget == true )
            {
                buildingTarget = false;
                
                // George:
                // not sure whether this is suppose to stop laser beacons that are on a corner
                // from not becoming foreground features as well?? if it is, I don't think it
                // works, so will disable it and just check minReturns_.
                // SBW: actually, this is meant to check that the return is not on a
                // corner.  The cor
                // again make sure that the feature is not a foreground point
                //if ( fabs( laserDataPtr.range(i) -  laserDataPtr.range(i+1) ) < config_.backgroundRangeGate && featureNumPnts >= minReturns_ )

                if ( featureNumPnts >= config_.minReturnNumber )
                {                 
                    orca::SinglePolarFeature2dPtr pp = new orca::SinglePolarFeature2d;
                    pp->type = orca::feature::LASERREFLECTOR;
                    pp->p.r  = featureRange / featureNumPnts;
                    pp->p.o  = featureBearing / featureNumPnts;
                    featureDataPtr->features.push_back( pp );
                }
            } // end of end-of-cluster if

        }     // end for each return
    }
    return 0;  
}

bool CombinedDriver::extractForegroundPoints( const orca::RangeScannerConfigPtr & laserConfigPtr,
                                              const orca::LaserDataPtr & laserDataPtr,
                                              const orca::PolarFeature2dDataPtr & featureDataPtr)
{
    std::vector<orca_polefinder::positionRB> poles;

    double minAngleFromDodge = DEG2RAD( 2.0 );

    int numPoles = orca_polefinder::detect_poles( laserConfigPtr,
                                                  laserDataPtr,
                                                  laserRange_,
                                                  config_.minForegroundWidth,
                                                  config_.maxForegroundWidth,
                                                  config_.minForegroundBackgroundSeparation,
                                                  minAngleFromDodge,
                                                  poles );
    
    for ( int i=0; i < numPoles; i++ )
    {
        orca::SinglePolarFeature2dPtr pp = new orca::SinglePolarFeature2d;
        pp->type = orca::feature::FOREGROUNDPOINT;
        pp->p.r  = poles[i].range;
        pp->p.o  = poles[i].bearing;
        featureDataPtr->features.push_back(pp);
    }

    return true;

}

bool CombinedDriver::extractDoors(const orca::LaserDataPtr & laserDataPtr,
                                  const orca::PolarFeature2dDataPtr & featureDataPtr)
{
    unsigned int i;
    bool buildingTarget = false;

    int featureNumPnts = 0;
  
    double delta_range = 0;
    double door_width_sq;
    double startBearing = - M_PI/2;
    double startRange = -1.0;
    double stopBearing = - M_PI/2;
    double stopRange = -1.0;
    double startX = 0.0;
    double startY =0.0;
    double stopX, stopY;
    double min_width = 0.9*0.9;
    double max_width = 1.5*1.5;

    //std::cout << "Looking for open doors..." << std::endl;
    // for each return
    for ( i=10; i<laserDataPtr->ranges.size()-10; i++ )
    {
        // compute the step change in range
        delta_range = laserDataPtr->ranges[i] - laserDataPtr->ranges[i-1];
        
        if (buildingTarget == true)
        {
            // if we have reached the end of a cluster, process a new observation
            if ( delta_range < -2.0 )
            {
                buildingTarget = false;
                stopRange = laserDataPtr->ranges[i];
                stopBearing = M_PI*(i)/(laserDataPtr->ranges.size()-1) - M_PI/2;
                stopX = stopRange*cos(stopBearing);
                stopY = stopRange*sin(stopBearing);

                
                door_width_sq = pow(stopX - startX, 2.0) + pow(stopY - startY, 2.0);
                
                //std::cout << " Checking a new cluster at range " << featureRange / featureNumPnts << " bearing " << featureBearing / featureNumPnts << " of width " << pole_width << " with " << featureNumPnts << " points" << std::endl;
                // again make sure that the feature is not a foreground point
                if (featureNumPnts > 5 && door_width_sq > min_width &&
                    door_width_sq < max_width )
                {
                    orca::SinglePolarFeature2dPtr pp1 = new orca::SinglePolarFeature2d;
                    pp1->type = orca::feature::DOOR;
                    pp1->p.r = startRange;
                    pp1->p.o = startBearing;

                    orca::SinglePolarFeature2dPtr pp2 = new orca::SinglePolarFeature2d;
                    pp2->type = orca::feature::DOOR;
                    pp2->p.r = stopRange;
                    pp2->p.o = stopBearing;
                    
                    featureDataPtr->features.push_back(pp1);
                    featureDataPtr->features.push_back(pp2);
                }
                
                buildingTarget = false;
            } else {
                featureNumPnts++;
            }  // end of end-of-cluster if
        }     // end for each return
        if ( delta_range > 2.0 )
        {
            // start building a new door shape
            if ( buildingTarget == false)
            {
                buildingTarget = true;
                featureNumPnts = 1;
                startRange = laserDataPtr->ranges[i-1];
                startBearing = M_PI*(i-1)/(laserDataPtr->ranges.size()-1) - M_PI/2;
                startX = startRange*cos(startBearing);
                startY = startRange*sin(startBearing);
                continue; // go to the next return
            }
        }
    }
    return true;

}

bool
CombinedDriver::extractCorners( const orca::LaserDataPtr & laserDataPtr,
                                const orca::PolarFeature2dDataPtr & featureDataPtr)
{
    //calculatePos(scan);
    sections_.clear();
    connectSections(laserDataPtr);
    extractLines();

    //vector<Section>::iterator itr;
    std::vector<Section>::iterator itr;
    std::vector<Section>::iterator next;
  
    for (itr = sections_.begin(), next = sections_.begin() + 1; itr != sections_.end() && next != sections_.end(); itr++, next++)
    {
        if (itr->isALine && itr->isNextCon && next->isALine) {
            // We have a corner
            double A1 = itr->A;
            double B1 = itr->B;
            double C1 = itr->C;

            double A2 = next->A;
            double B2 = next->B;
            double C2 = next->C;

            double dot_prod = A1*A2 + B1*B2;
            //cout << "dot_prod: " << dot_prod << endl;
            if (fabs(dot_prod) < CORNER_BOUND) {
                double cornerX;
                double cornerY;
                if (B1 == 0) {
                    cornerX = -C1/A1;
                    cornerY = (-C2 + A2*C1/A1)/B2;
                }
                else {
                    cornerX = ((B2*C1/B1) - C2)/(A2 - 
                                                 B2*A1/B1);
                    cornerY = (-A1*cornerX - C1)/B1;
                }
                double range = sqrt(pow(cornerX,2) + pow(cornerY,2));
                double bearing = acos(cornerX/range);
                if (cornerY < 0) {
                    bearing = -bearing;
                }
                orca::SinglePolarFeature2dPtr pp = new orca::SinglePolarFeature2d;
                pp->type = orca::feature::CORNER;
                pp->p.r = range;
                pp->p.o = bearing;
                featureDataPtr->features.push_back( pp );
            }
        }
        //itr = itr->next;
    }
    //
    // Now we can get the possible corners...
    //
//  extractPossibleCorners(features);
    return true;

}

void CombinedDriver::connectSections(const orca::LaserDataPtr & laserDataPtr)
{
    //Section *head = new Section();
    //Section *current = head;
    Section current;
  
    bool inSection = true;
    if (laserDataPtr->ranges[0] >= laserRange_) {
        inSection = false;
    } else {
        double r = laserDataPtr->ranges[0];
        double b = - M_PI/2;
        SectionEl pos(r, b);
        current.elements.push_back(pos);
    }

    for (uint i = 1; i < laserDataPtr->ranges.size(); i++) {
        if (laserDataPtr->ranges[i] >= laserRange_) {
            if (laserDataPtr->ranges[i-1] < laserRange_) {
                //Section temp;
                sections_.push_back(current);
                current.elements.clear();
            } else {
                // We are still in an out of range section
                // ignore...
            }
        } else if (laserDataPtr->ranges[i-1] >= laserRange_ ||
                   fabs(laserDataPtr->ranges[i] - laserDataPtr->ranges[i-1]) < RANGE_DELTA)  {
            // Add this point to the current section
            double r = laserDataPtr->ranges[i];
            double b = M_PI*i/(laserDataPtr->ranges.size()-1) - M_PI/2;
            SectionEl pos(r, b);
            current.elements.push_back(pos);
        }
        else {
            // We should start a new section
            //Section temp;
            sections_.push_back(current);
            current.elements.clear();
            //temp.elements.push_back(&(scan.scan[i]));
            //sections_.push_back(temp);
        }
    }

    //std::cout << "FeatureExtractor : Found " << sections_.size() << " sections" << std::endl;
    //printSections();
    //
    // If the last thing we saw was out of max range, then remove the
    //  last entry. It is invalid
    //
    //if (current.elements.size() == 0) {
    //  sections_.pop_back();
    //}
    //return head;
}

void CombinedDriver::extractLines()
{
    //std::cout << "Extracting lines from " << sections_.size() << " initial sections" << std::endl;
  
    //for (std::vector<Section>::iterator itr = sections_.begin(); itr != sections_.end(); itr++) {
    std::vector<Section>::iterator itr = sections_.begin();
  
    while (itr != sections_.end())
    {
        if (itr->elements.size() >= MIN_POINTS_IN_LINE) {
            int pos = 0;
            double maxDist = 0;
            //std::cout << "Preparing to find breaks for section of size << " << itr->elements.size() << std::endl;
            Section sec = *itr;
            //findBreakPoint(*itr, maxDist, pos);
            findBreakPoint(sec, maxDist, pos);

            if (maxDist > BREAK_DIST) {
                //std::cout << "Found a break point at position " << pos << " in section of size " << itr->elements.size() << std::endl;
        
                Section temp;
                // insert at...
                //temp->next = itr->next;
                //itr->next = temp;
                temp.isNextCon = true;
        
                temp.elements.insert( temp.elements.begin(), itr->elements.begin(), itr->elements.begin() + pos);
        
                itr->elements.erase( itr->elements.begin(), itr->elements.begin() + pos );
        
                //std::cout << "Inserting section of size " << temp.elements.size() << " after section of size " << itr->elements.size() << std::endl;
                try {
                    // insert the new section and get a valid handle to the inserted section
                    itr = sections_.insert(itr, temp);
                    //itr--;
                } catch(...) {
                    std::cout << "Exception in inserting new section!!!" << std::endl;
                }
                //printSections();
                //sections_.insert(itr+1, temp);
                //itr++;
            } else {
                // std::cout << "Preparing to fit lines" << std::endl;
                fitLine(*itr);
                //itr = itr->next;in 
                itr++;
            }
        } else {
            // std::cout << "Not breaking this one..." << std::endl;
            //itr = itr->next;
            itr++;
        }
    }
    //std::cout << "FeatureExtractor : Extracted " << sections_.size() << " lines" << std::endl;

}

void CombinedDriver::findBreakPoint(Section &s, double &maxDist, int &pos)
{
    maxDist = 0;
    pos = 0;
  
    int sectionLength = s.elements.size();
    if ( sectionLength <= 2) {
        return;
    }
  
    if (s.elements[0].x() - s.elements[sectionLength-1].x() == 0) {
        // They have same x values, hence infinite slope.
        // Do a quicker check...

        //std::cout << "Same x value " << length << std::endl;
    
        pos = 0;
        maxDist = 0;
        double lineY = s.elements[0].y();
        for (int i = 1; i < sectionLength - 1; i++) {
    
            double dist = fabs(lineY - s.elements[i].y());
            if (dist > maxDist) {
                maxDist = dist;
                pos = i;
            }
        }
        return;
    }
    // else
    //std::cout << "Non-infinite slope" << std::endl;

    double deltaX = s.elements[sectionLength-1].x() - s.elements[0].x();
    double deltaY = s.elements[sectionLength-1].y() - s.elements[0].y();
    //std::cout << "dx " << deltaX << " dy " << deltaY;
  
    double A = deltaY/deltaX;
    double C = s.elements[0].y() - A*s.elements[0].x();
    //std::cout  <<  "A " << A  << " C " << C;
  
    double norm = sqrt(1 + pow(A, 2));
    //std::cout << " norm " << norm;
    A = A/norm;
    double B = -1/norm;
    C = C/norm;

    //std::cout  <<  "nA " << A << " nB " << B << " nC " << C << std::endl;
      
    //std::cout << "Looking for max distance: ";
    for (int i = 1; i < sectionLength - 2; i++) {
        double dist = fabs(A*s.elements[i].x() 
                           + B*s.elements[i].y() + C);
        //std::cout << dist << " ";

        if (dist > maxDist) {
            maxDist = dist;
            pos = i;
        }
    }
    //std::cout <<  "Found max distance " << maxDist << std::endl;
}

void CombinedDriver::fitLine(Section &s)
{
    double centX = 0;
    double centY = 0;
    int n = 2;
    int m = s.elements.size();

    // Find the centroid of the section..
    //std::cout << "Finding the centroid of the section" << std::endl;

    for (int i = 0; i < m; i++) {
        centX += s.elements[i].x();
        centY += s.elements[i].y();
    }
    centX = centX/(double)m;
    centY = centY/(double)m;
  
    float **A = new float*[m];
    for (int i = 0; i < m; i++) {
        A[i] = new float[n];
    }

    float *W = new float[n];

    float **V = new float*[n];
    for (int i = 0; i < n; i++) {
        V[i] = new float[n];
    }

    for (int i = 0; i < m; i++) {
        A[i][0] = s.elements[i].x() - centX;
        A[i][1] = s.elements[i].y() - centY;
    }

    //std::cout << "Calling SVD" << std::endl;
    dsvd(A, m, n, W, V);

    int minIndex = 0;
    for (int i = 0; i < n; i++) {
        if (fabs(W[i]) < fabs(W[minIndex])) {
            minIndex = i;
        }
    }

    s.A = V[0][minIndex];
    s.B = V[1][minIndex];
    s.C = -s.A*centX - s.B*centY;

    double total_error = 0;
    for (int i = 0; i < m; i++) {
        double dist = fabs(s.A*s.elements[i].x() 
                           + s.B*s.elements[i].y() + s.C);

        total_error += dist;
    }
    if (total_error/(double)m < ERROR_THRESHOLD) {
        s.isALine = true;
    }
    else {
        cout << "Unable to bound line: error " << total_error/(double)m << endl;
    }

    // Cleanup the arrays...
    delete A;

    delete W;
    delete V;
}

void CombinedDriver::printSections()
{
    int count = 0;

    cout << "Sections : ";
    for (std::vector<Section>::iterator itr = sections_.begin(); itr != sections_.end(); itr++) {
        count += itr->elements.size();
        cout << "{s:" << itr->elements.size();
        if (itr->isALine) {
            cout << " L";
        }
        else {
            cout << " -";
        }
        if (itr->isNextCon) {
            cout << "C";
        }
        else {
            cout << "-";
        }

        cout << "}  ";
        //itr = itr->next;
    }
    cout << " - Count: " << count << endl;
}
    
bool CombinedDriver::extractPossibleCorners( const orca::PolarFeature2dDataPtr & featureDataPtr )
{
//  return false; // for now, we won't be looking for possible corners...

    if (sections_.size() <= 1) 
        return false;

    //Section *itr = sec->next;
    //Section *prev = sec;
    std::vector<Section>::iterator prev;
    std::vector<Section>::iterator itr;
  
    for (prev = sections_.begin(), itr = sections_.begin() + 1; itr != sections_.end() && prev != sections_.end(); itr++, prev++) 
    {
        if ( prev->elements.size() > 0 && itr->elements.size() > 0)
        {
            //while (itr != NULL) {
            // check the end of the previous line 
            if (!prev->isNextCon && prev->isALine) {
                SectionEl pret = prev->elements.back();
                SectionEl iret = itr->elements.front();
                if (iret.range() > pret.range() + POSSIBLE_BOUND) {
                    orca::SinglePolarFeature2dPtr pp = new orca::SinglePolarFeature2d;
                    pp->type = orca::feature::POSSIBLECORNER;
                    pp->p.r = pret.range();
                    pp->p.o = pret.bearing();
                    featureDataPtr->features.push_back( pp );
                }
            }
  
            // Check the start of the line for possible corner...
            if (itr->isALine && !prev->isNextCon) {
                SectionEl pret = prev->elements.back();
                SectionEl iret = itr->elements.front();
                if (pret.range() > iret.range() + POSSIBLE_BOUND) {
                    orca::SinglePolarFeature2dPtr pp = new orca::SinglePolarFeature2d;
                    pp->type = orca::feature::POSSIBLECORNER;
                    pp->p.r = iret.range();
                    pp->p.o = iret.bearing();
                    featureDataPtr->features.push_back( pp );
                }
            }
        }
    }
    return true;
}
