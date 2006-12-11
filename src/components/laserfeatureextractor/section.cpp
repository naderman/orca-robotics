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

#include <iostream>
#include "section.h"
#include "svd.h"

using namespace std;

namespace {
    const double LINEFIT_ERROR_THRESHOLD = 0.24;    // 0.10    0.24
}

Section::Section()
{
	//next = NULL;
	isNextCon_ = false;

	isALine_ = false;
	eigVectX_ = 0.0;
	eigVectY_ = 0.0;
	c_ = 0.0;
}


Section::~Section()
{
	// Nothing to delete
}

double 
Section::lineLength() const
{
    assert( isALine() );
    return hypotf( start().y()-end().y(), start().x()-end().x() );
}

void
Section::setEndPoints()
{
    // calc length using the actual endpoints
    const SectionEl &s = elements_.front();
    const SectionEl &e = elements_.back();
    double halfLength = 0.5 * hypotf( s.y()-e.y(), s.x()-e.x() );

    double vParallelX = -eigVectY_;
    double vParallelY =  eigVectX_;

    start_.setXY( centroidX_ - vParallelX*halfLength, centroidY_ - vParallelY*halfLength );
    end_.setXY(   centroidX_ + vParallelX*halfLength, centroidY_ + vParallelY*halfLength );

    // Make sure the points are ordered correctly.
    if ( end_.bearing()-start_.bearing() < 0 )
    {
        // swap them
        SectionEl temp = start_;
        start_ = end_;
        end_ = temp;
    }

    // Make sure the points are ordered correctly.
    assert( end_.bearing()-start_.bearing() > 0 );
}

void 
extractSections( const std::vector<float> &ranges,
                 double angleStart,
                 double angleIncrement,
                 double maxRange,
                 double minPointsInLine,
                 double maxRangeDelta,
                 std::vector<Section> &sections )
{
    Section current;
  
    bool inSection = true;
    if (ranges[0] >= maxRange) {
        inSection = false;
    } else {
        double r = ranges[0];
        double b = - M_PI/2;
        SectionEl pos(r, b);
        current.elements().push_back(pos);
    }

    for (unsigned int i = 1; i < ranges.size(); i++) {
        if (ranges[i] >= maxRange) {
            if (ranges[i-1] < maxRange) {
              // found the end of the current section with an out of range reading
              // ignore elements with less candidate points than would constitute 2 lines
              if (current.elements().size() > 2*minPointsInLine)
              {
                sections.push_back(current);
              }
              current.elements().clear();
            } else {
                // We are still in an out of range section
                // ignore...
            }
        } else if (ranges[i-1] >= maxRange ||
                   fabs(ranges[i] - ranges[i-1]) < maxRangeDelta) 
        {
            // Add this point to the current section
            double r = ranges[i];
            double b = M_PI*i/(ranges.size()-1) - M_PI/2;
            SectionEl pos(r, b);
            current.elements().push_back(pos);
         } else {
           // There has been a step change in range, start a new section
           // ignore elements with less candidate points than would constitute 2 lines
           if (current.elements().size() > 2*minPointsInLine)
           {
             sections.push_back(current);
           }
           current.elements().clear();
         }
    }

    if (current.elements().size() > 2*minPointsInLine)
    {
      sections.push_back(current);
    }
    current.elements().clear();
    
    //std::cout << "FeatureExtractor : Found " << sections.size() << " sections" << std::endl;
    //printSections();
}

void 
Section::tryFitLine()
{
    int n = 2;
    int m = elements_.size();

    // Find the centroid of the section..
    //std::cout << "Finding the centroid of the section" << std::endl;

    for (int i = 0; i < m; i++) {
        centroidX_ += elements_[i].x();
        centroidY_ += elements_[i].y();
    }
    centroidX_ /= (double)m;
    centroidY_ /= (double)m;
  
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
        A[i][0] = elements_[i].x() - centroidX_;
        A[i][1] = elements_[i].y() - centroidY_;
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
    eigVectX_ = V[0][minIndex];
    eigVectY_ = V[1][minIndex];
    c_ = -eigVectX_*centroidX_ - eigVectY_*centroidY_;

    double total_error = 0;
    for (int i = 0; i < m; i++) {
        double dist = fabs(eigVectX_*elements_[i].x() 
                           + eigVectY_*elements_[i].y() + c_);

        total_error += dist;
    }
    if (total_error/(double)m < LINEFIT_ERROR_THRESHOLD) 
    {
        isALine_ = true;
        setEndPoints();
    }
    else {
        cout << "Unable to bound line: error " << total_error/(double)m << endl;
    }

    // Cleanup the arrays...
    delete A;

    delete W;
    delete V;
}

void 
Section::findBreakPoint(double &maxDist, int &pos) const
{
    maxDist = 0;
    pos = 0;
  
    int numElements = elements_.size();
    if ( numElements <= 2) {
        return;
    }
    double x1 = elements_[0].x();
    double y1 = elements_[0].y();
    double xn = elements_[numElements-1].x();
    double yn = elements_[numElements-1].y();
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
        double x = elements_[i].x();
        double y = elements_[i].y();
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

void 
extractLines( std::vector<Section> &sections, int minPointsInLine )
{
    const double BREAK_DIST = 0.2;

    //std::cout << "Extracting lines from " << sections.size() << " initial sections" << std::endl;
  
    std::vector<Section>::iterator itr = sections.begin();
  
    while (itr != sections.end())
    {
        if ( itr->elements().size() < (uint)(minPointsInLine) )
        {
            // Don't break this one.
            itr++;
            continue;
        }

        int pos = 0;
        double maxDist = 0;
        //std::cout << "Preparing to find breaks for section of size << " << itr->elements.size() << std::endl;
        (*itr).findBreakPoint(maxDist, pos);

        if (maxDist > BREAK_DIST) {
            //std::cout << "Found a break point at position " << pos << " in section of size " << itr->elements.size() << std::endl;
        
            Section temp;
            temp.setIsNextCon( true );
        
            temp.elements().insert( temp.elements().begin(), itr->elements().begin(), itr->elements().begin() + pos);
        
            itr->elements().erase( itr->elements().begin(), itr->elements().begin() + pos );
        
            //std::cout << "Inserting section of size " << temp.elements().size() << " before section of size " << itr->elements().size() << std::endl;
            try {
                // insert the new section and get a valid handle to the inserted section
                itr = sections.insert(itr, temp);
                //itr--;
            } catch(...) {
                std::cout << "Exception in inserting new section!!!" << std::endl;
            }
        } else {
            // std::cout << "Preparing to fit lines" << std::endl;
            (*itr).tryFitLine();
            itr++;
        }
    }
    //std::cout << "FeatureExtractor : Extracted " << sections.size() << " lines" << std::endl;
}

void printSections( const std::vector<Section> &sections )
{
    int count = 0;

    cout << "Sections : ";
    for (std::vector<Section>::const_iterator itr = sections.begin(); itr != sections.end(); itr++) {
        count += itr->elements().size();
        cout << "{s:" << itr->elements().size();
        if (itr->isALine()) {
            cout << " L";
        }
        else {
            cout << " -";
        }
        if (itr->isNextCon()) {
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

