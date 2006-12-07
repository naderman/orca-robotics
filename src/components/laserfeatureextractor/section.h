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

    void tryFitLine();

    // Is this section connected to the next?
    bool isNextCon() const { return isNextCon_; }

    // Are the points linear enough to be considered a line?
    bool isALine() const { return isALine_; }

    // centroid of the line segment
    double centroidX() const { assert(isALine()); return centroidX_; }
    double centroidY() const { assert(isALine()); return centroidY_; }

    // vector perpendicular to line segment.
    // guaranteed to be of length 1.
    double perpVectX() const { assert(isALine()); return eigVectX_; }
    double perpVectY() const { assert(isALine()); return eigVectY_; }

    //  Line equation is: eigVectX*x - eigVectY*y + c = 0
    double c() const { assert(isALine()); return c_; }

    // Get end-points of line (using entire estimate from all points)
    const SectionEl &start() const { assert(isALine()); return start_; }
    const SectionEl &end() const { assert(isALine()); return end_; }

    // length of segment (using entire estimate from all points)
    double lineLength() const;

    // Try to find a point to break a line in two
    void findBreakPoint(double &maxDist, int &pos) const;

    const std::vector<SectionEl> &elements() const { return elements_; }

    // extractLines needs to access some stuff...
    std::vector<SectionEl> &elements() { return elements_; }
    void setIsNextCon( bool val ) { isNextCon_ = val; }

private:

    void setEndPoints();

    bool isNextCon_;

    bool isALine_;

    double centroidX_;
    double centroidY_;

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

// Try to fit lines to the sections provided.
void extractLines( std::vector<Section> &sections, int minPointsInLine );

// Print to cout
void printSections( const std::vector<Section> &sections );

#endif  // LOCALISER_SECION_H
