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
#include "cornerextractor.h"
#include <orcaice/orcaice.h>

#include <iostream>
#include <assert.h>
#include <cmath>

#include "svd.h"

#include <orca/featuremap2d.h>

#define RANGE_DELTA     0.5    // 0.10    0.2
#define BREAK_DIST     0.2    // 0.4    0.2
#define MIN_POINTS_IN_LINE   6    // 6      6
#define MAX_POINTS_IN_LINE   20   // 360    25
#define ERROR_THRESHOLD   0.24    // 0.10    0.24
#define CORNER_BOUND     0.2    // 0.45    0.45
#define POSSIBLE_BOUND    0.2    // ???    0.2
#define REJECT_GROUND_OBSERVATIONS 1  // 0  1


using namespace std;

namespace laserfeatures {

namespace {
    const double P_FALSE_POSITIVE = 0.3;
    const double P_FALSE_POSITIVE_POSSIBLE_GROUND = 0.5;
    const double P_TRUE_POSITIVE  = 0.6;
}

double lineLength( const Section &s )
{
    const SectionEl &start = s.elements[0];
    const SectionEl &end   = s.elements[s.elements.size()-1];

    // A bit unstable if the end-points are screwy, but should be mostly OK...
    return hypotf( start.y()-end.y(), start.x()-end.x() );
}

CornerExtractor::CornerExtractor( orcaice::Context context, double laserMaxRange )
    : laserMaxRange_( laserMaxRange )
{
    std::string prefix = context.tag() + ".Config.Reflectors.";
    Ice::PropertiesPtr prop = context.properties();

    minLineLength_ = orcaice::getPropertyAsDoubleWithDefault( prop, prefix+"MinLineLength", 1.0 );
}
    
void CornerExtractor::addFeatures( const orca::LaserScanner2dDataPtr &laserData,
                                   orca::PolarFeature2dDataPtr &features )
{  
    assert( laserMaxRange_ > 0.0 );
    
    //calculatePos(scan);
    sections_.clear();
    connectSections(laserData);
    extractLines();

    //vector<Section>::iterator itr;
    std::vector<Section>::iterator itr;
    std::vector<Section>::iterator next;
  
    for (itr = sections_.begin(), next = sections_.begin() + 1; itr != sections_.end() && next != sections_.end(); itr++, next++)
    {
        // check that we have two connected lines before computing the angle between them
        // We also want to avoid extracting corners at the junction between observations of the
        // ground and a wall.
        if (itr->isALine && itr->isNextCon && next->isALine) {// &&
            //(itr->elements.size() < MAX_POINTS_IN_LINE) &&
            //(next->elements.size() < MAX_POINTS_IN_LINE)) {

            if ( lineLength( *itr ) < minLineLength_ )
            {
                cout<<"TRACE(cornerextractor.cpp): line too small: length is " << lineLength(*itr) << endl;
                continue;
            }
            if ( lineLength( *next ) < minLineLength_ )
            {
                cout<<"TRACE(cornerextractor.cpp): line too small: length is " << lineLength(*next) << endl;
                continue;
            }

            double pFalsePositive = P_FALSE_POSITIVE;

            if (REJECT_GROUND_OBSERVATIONS && 
            ((itr->elements.size() > MAX_POINTS_IN_LINE && fabs(itr->eigVectY) < 0.1) ||
            (next->elements.size() > MAX_POINTS_IN_LINE && fabs(next->eigVectY) < 0.1))) 
            {
              //std::cout << "We have a big line with a near horizontal slope.  Could be the ground??? Slope A : " << itr->eigVectY << " Slope B : " << next->eigVectY << std::endl;
              pFalsePositive = P_FALSE_POSITIVE_POSSIBLE_GROUND;
            }
  
            // We have a corner
            double A1 = itr->eigVectX;
            double B1 = itr->eigVectY;
            double C1 = itr->C;
  
            double A2 = next->eigVectX;
            double B2 = next->eigVectY;
            double C2 = next->C;
  
            double dot_prod = A1*A2 + B1*B2;
              
            // the dot product of the two unit vectors will be 0 if the lines are perpendicular
            if (fabs(dot_prod) < CORNER_BOUND) {
                double cornerX;
                double cornerY;
  
                // use Kramer's rule to find the intersection point of the two lines
                // The equations for the lines are:
                // A1 x + B1 y + C1 = 0 and
                // A2 x + B2 y + C2 = 0
                //
                // We need to solve for the (x, y) that satisfies both equations
                //
                // x = | C1 B1 |  / | A1 B1 | 
                //     | C2 B2 | /  | A2 B2 |
                //
                // y = | A1 C1 |  / | A1 B1 | 
                //     | A2 C2 | /  | A2 B2 |
  
                double den = (B1*A2 - B2*A1);
                  
                // this check isn't really necessary as we have already established that the lines
                // are close to perpendicular.  The determinant will only be zero if the lines
                // are parallel
                if (den != 0)
                {
                    cornerX = (B2*C1 - B1*C2)/den;
                    cornerY = (A1*C2 - A2*C1)/den;
    
                    double range = sqrt(pow(cornerX,2) + pow(cornerY,2));
                    double bearing = acos(cornerX/range);
                    if (cornerY < 0) {
                        bearing = -bearing;
                    }
    
                    orca::SinglePolarFeature2dPtr pp = new orca::SinglePolarFeature2d;
                    pp->type = orca::feature::CORNER;
                    pp->p.r = range;
                    pp->p.o = bearing;
                    pp->pFalsePositive = pFalsePositive;
                    pp->pTruePositive  = P_TRUE_POSITIVE;
                    features->features.push_back( pp );
                }
            }
        }
        //itr = itr->next;
    }
    //
    // Now we can get the possible corners...
    //
//  extractPossibleCorners(features);  
 
}

void CornerExtractor::connectSections(const orca::LaserScanner2dDataPtr & laserDataPtr)
{
    //Section *head = new Section();
    //Section *current = head;
    Section current;
  
    bool inSection = true;
    if (laserDataPtr->ranges[0] >= laserMaxRange_) {
        inSection = false;
    } else {
        double r = laserDataPtr->ranges[0];
        double b = - M_PI/2;
        SectionEl pos(r, b);
        current.elements.push_back(pos);
    }

    for (unsigned int i = 1; i < laserDataPtr->ranges.size(); i++) {
        if (laserDataPtr->ranges[i] >= laserMaxRange_) {
            if (laserDataPtr->ranges[i-1] < laserMaxRange_) {
              // found the end of the current section with an out of range reading
              // ignore elements with less candidate points than would constitute 2 lines
              if (current.elements.size() > 2*MIN_POINTS_IN_LINE)
              {
                sections_.push_back(current);
              }
              current.elements.clear();
            } else {
                // We are still in an out of range section
                // ignore...
            }
        } else if (laserDataPtr->ranges[i-1] >= laserMaxRange_ ||
                   fabs(laserDataPtr->ranges[i] - laserDataPtr->ranges[i-1]) < RANGE_DELTA) 
        {
            // Add this point to the current section
            double r = laserDataPtr->ranges[i];
            double b = M_PI*i/(laserDataPtr->ranges.size()-1) - M_PI/2;
            SectionEl pos(r, b);
            current.elements.push_back(pos);
         } else {
           // There has been a step change in range, start a new section
           // ignore elements with less candidate points than would constitute 2 lines
           if (current.elements.size() > 2*MIN_POINTS_IN_LINE)
           {
             sections_.push_back(current);
           }
           current.elements.clear();
         }
    }

    if (current.elements.size() > 2*MIN_POINTS_IN_LINE)
    {
      sections_.push_back(current);
    }
    current.elements.clear();
    
    //std::cout << "FeatureExtractor : Found " << sections_.size() << " sections" << std::endl;
    //printSections();
}

void CornerExtractor::extractLines()
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
        
                //std::cout << "Inserting section of size " << temp.elements.size() << " before section of size " << itr->elements.size() << std::endl;
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
            //std::cout << "Not breaking this one..." << std::endl;
            //itr = itr->next;
            itr++;
        }
    }
    //std::cout << "FeatureExtractor : Extracted " << sections_.size() << " lines" << std::endl;

}

void CornerExtractor::findBreakPoint(Section &s, double &maxDist, int &pos)
{
  maxDist = 0;
  pos = 0;
  
  int numElements = s.elements.size();
  if ( numElements <= 2) {
    return;
  }
  double x1 = s.elements[0].x();
  double y1 = s.elements[0].y();
  double xn = s.elements[numElements-1].x();
  double yn = s.elements[numElements-1].y();
  double deltaX =  xn - x1;
  double deltaY =  yn - y1;
  //std::cout << "dx " << deltaX << " dy " << deltaY;
  double sectionLength = sqrt(pow(deltaX, 2.0) + pow(deltaY, 2.0));
  if (sectionLength == 0.0)
  {
    return;
  }
  
  // compute the perpendicular distance between the line joining the first and
  // last elements of the section and each point to find the maximum distance.  If
  // this distance is greater than a threshold it will be used to break the section
  // into two smaller sections
  for (int i = 1; i < numElements - 2; i++) {
    double x = s.elements[i].x();
    double y = s.elements[i].y();
    double dx = x - x1;
    double dy = y - y1;

    // the distance is computed using the dot product between the vector 
    // joining the current point and the first endpoint 
    // r = [ x - x1 ]
    //     [ y - y1 ]
    // and the vector perpendicular to the line segment
    // v = [  yn - y1   ]
    //     [ -(xn - y1) ]
    // The distance is
    //   d =  | v.r | 
    //     =  | ( x - x1 )( yn - y1) - ( y - y1 )( xn - x1) | / sqrt((xn-x1)^2+(yn-y1)^2)
    double r = dx*deltaY - dy*deltaX;
    double dist = fabs(r/sectionLength);
    //cout << "Distance : " << dist << " dist_r " << dist_r << endl;

    if (dist > maxDist) {
      maxDist = dist;
      pos = i;
    }
  }

  //std::cout <<  "Found max distance " << maxDist << std::endl;
}

    
void CornerExtractor::fitLine(Section &s)
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

    // the equation for the line is defined as:
    //
    //  vx x - vy y + c = 0
    //  vx x - vy y + (vy y0 + vx x0) = 0
    //
    //   where vx is the first component of the eigenvector
    //         vy is the second component of the eigencector
    //         c is the intercept
    s.eigVectX = V[0][minIndex];
    s.eigVectY = V[1][minIndex];
    s.C = -s.eigVectX*centX - s.eigVectY*centY;

    double total_error = 0;
    for (int i = 0; i < m; i++) {
        double dist = fabs(s.eigVectX*s.elements[i].x() 
                    + s.eigVectY*s.elements[i].y() + s.C);

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

void CornerExtractor::printSections()
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
    
bool CornerExtractor::extractPossibleCorners( const orca::PolarFeature2dDataPtr & featureDataPtr )
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
                    pp->pFalsePositive = P_FALSE_POSITIVE;
                    pp->pTruePositive  = P_TRUE_POSITIVE;
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
                    pp->pFalsePositive = P_FALSE_POSITIVE;
                    pp->pTruePositive  = P_TRUE_POSITIVE;
                    featureDataPtr->features.push_back( pp );
                }
            }
        }
    }
    return true;
}

}
