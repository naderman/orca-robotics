#include "cornerextractor.h"

#include <iostream>
#include <assert.h>
#include <cmath>

#include "svd.h"

#include <orca/featuremap2d.h>

// Stage values....Real Values
#define RANGE_DELTA     0.5    // 0.10    0.2
#define BREAK_DIST     0.2    // 0.4    0.2
#define MIN_POINTS_IN_LINE   6    // 6      6
#define ERROR_THRESHOLD   0.24    // 0.10    0.24
#define CORNER_BOUND     0.2    // 0.45    0.45
#define POSSIBLE_BOUND    0.2    // ???    0.2


using namespace std;

namespace laserfeatures {
    
void CornerExtractor::addFeatures( const orca::LaserDataPtr &laserData,
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
        if (itr->isALine && itr->isNextCon && next->isALine) {
            // We have a corner
            double A1 = itr->A;
            double B1 = itr->B;
            double C1 = itr->C;

            double A2 = next->A;
            double B2 = next->B;
            double C2 = next->C;

            double dot_prod = A1*A2 + B1*B2;
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
                features->features.push_back( pp );
            }
        }
        //itr = itr->next;
    }
    //
    // Now we can get the possible corners...
    //
//  extractPossibleCorners(features);  
 
}

void CornerExtractor::connectSections(const orca::LaserDataPtr & laserDataPtr)
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

}
