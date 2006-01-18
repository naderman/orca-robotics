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
  double A;
  double B;
  double C;

  std::vector<SectionEl> elements;
  //std::vector<orca::Point2D> elements;
  //std::vector<LaserReturn *> elements;
  //LaserReturn *getFirstElement();
  //LaserReturn *getLastElement();
};


#endif  // LOCALISER_SECION_H
