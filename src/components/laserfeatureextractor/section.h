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
  
  double x() { return x_; };
  double y() { return y_; };
  
  double range() { return r_; };
  double bearing() { return b_; }

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

  //Section *next;
  bool isNextCon;

  bool isALine;
  double eigVectX;
  double eigVectY;
  double C;

  std::vector<SectionEl> elements;
  //std::vector<orca::Point2D> elements;
  //std::vector<LaserReturn *> elements;
  //LaserReturn *getFirstElement();
  //LaserReturn *getLastElement();
};


#endif  // LOCALISER_SECION_H
