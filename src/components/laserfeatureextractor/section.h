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

#ifndef LOCALISER_SECTION_H
#define LOCALISER_SECTION_H

#include <vector>
#include <math.h>

class SectionEl
{
public:  
    SectionEl() {}
    SectionEl( double r, double b ) { r_ = r; b_ = b; x_ = r*cos(b); y_ = r*sin(b); };

    void setXY( double x, double y ) { r_ = hypotf(y,x); b_ = atan2(y,x); x_ = x; y_ = y; };
  
    double x() const { return x_; };
    double y() const { return y_; };
  
    double range() const { return r_; };
    double bearing() const { return b_; }

    std::string toStringXY() const;
    std::string toStringRB() const;

private:
    double x_;
    double y_;
    double r_;
    double b_;
};


class Section
{
public:

    Section();
    ~Section();

    // Is this section connected to the next?
    bool isNextCon() const { return isNextCon_; }

    // Are the points linear enough to be considered a line?
    bool isALine() const { return isALine_; }

    // vector perpendicular to line segment.
    // guaranteed to be of length 1.
    double eigVectX() const { assert(isALine()); return eigVectX_; }
    double eigVectY() const { assert(isALine()); return eigVectY_; }

    //  Line equation is: eigVectX*x - eigVectY*y + c = 0
    double c() const { assert(isALine()); return c_; }

    // Get end-points of line (using entire estimate from all points)
    const SectionEl &start() const { assert(isALine()); return start_; }
    const SectionEl &end() const { assert(isALine()); return end_; }

    // To help determine visibility of line endpoints, keep track
    // of the range on either side of the line segment.
    // For the first and last returns, the range either side is set to -1.
    double rangeBeforeStart() const { return rangeBeforeStart_; }
    double rangeAfterEnd()    const { return rangeAfterEnd_; }
    double &rangeBeforeStart() { return rangeBeforeStart_; }
    double &rangeAfterEnd()    { return rangeAfterEnd_; }

    // length of segment (using entire estimate from all points)
    double lineLength() const;

    const std::vector<SectionEl> &elements() const { return elements_; }

    // extractLines needs to access some stuff...
    std::vector<SectionEl> &elements() { return elements_; }
    void setIsNextCon( bool val ) { isNextCon_ = val; }

    void setIsALine( double eigVectX,
                     double eigVectY,
                     double c ) 
        { 
            isALine_=true; 
            eigVectX_=eigVectX;
            eigVectY_=eigVectY;
            c_=c;            
            setEndPoints(); 
        }

private:

    void setEndPoints();

    double rangeBeforeStart_;
    double rangeAfterEnd_;

    bool isNextCon_;

    bool isALine_;

    SectionEl centroid_;

    // eigVect describes a vector perpendicular to the line
    double eigVectX_;
    double eigVectY_;

    double c_;

    std::vector<SectionEl> elements_;

    // end-points of line (using entire estimate from all points)
    SectionEl start_;
    SectionEl end_;
};

//
// Utility functions
//

// Find sections from the raw data
void extractSections( const std::vector<float> &ranges,
                      double angleStart,
                      double angleIncrement,
                      double maxRange,
                      double minPointsInLine,
                      double maxRangeDelta,
                      std::vector<Section> &sections );

// Try to fit lines to the sections provided.
void extractLines( std::vector<Section> &sections, int minPointsInLine );

// Finds the point which maximises the projected distance to the line.
// Sets its index and the distance, or negative values if non found.
// Returns true if a break-point was found.
//
// TODO: This algorithm has problems...  In an 'L'-shaped segment,
//       it chooses to break near the lower-right rather than the lower-left.
//
bool findBreakPoint( const std::vector<SectionEl> &sectionElements,
                     double eigVectX,
                     double eigVectY,
                     double c,
                     double &maxDist,
                     int &pos );


// Split any bent sections into multiple smaller sections.
// breakDistThreshold says when to break: when the orthogonal projection
// onto the line is greater than this.
void breakAndFitLines( std::vector<Section> &sections,
                       int minPointsInLine,
                       double breakDistThreshold );

// The equation for the line is defined as:
//
//  vx x - vy y + c = 0
//  vx x - vy y + (vy y0 + vx x0) = 0
//
//   where vx is the first component of the eigenvector (perpendicular to line)
//         vy is the second component of the eigenvector (perpendicular to line)
//         c is the intercept
//
// The eigenvector is of unit length
//
void fitLine( const std::vector<SectionEl> &elements,
              double &eigVectX, double &eigVectY, double &c );

// Print to cout
void printSections( const std::vector<Section> &sections );

#endif  // LOCALISER_SECION_H
