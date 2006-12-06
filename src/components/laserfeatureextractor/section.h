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
  SectionEl( double r, double b ) { r_ = r; b_ = b; x_ = r*cos(b); y_ = r*sin(b); };
  
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

  bool isNextCon;

  bool isALine;
  double eigVectX;
  double eigVectY;
  double C;

  std::vector<SectionEl> elements;
};

//
// Utility functions
//

// Sets the line parameters and isALine
void fitLine( Section &s );

// Try to fit lines to the sections provided.
void extractLines( std::vector<Section> &sections, int minPointsInLine );

// Print to cout
void printSections( const std::vector<Section> &sections );

#endif  // LOCALISER_SECION_H
