/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

/***************************************************************************
                          cov2d.h  -  description
                             -------------------
    begin                : Thu Jun 27 2002
    copyright            : (C) 2002 by Alex Makarenko
    email                : a.makarenko@acfr.usyd.edu.au
    description          : 2D covariance matrix
                         : 
    versions             : v0 based Tim Baley's code
                         : v1 fixed version of ellipse with a,b,th
                         : v2 5/1/03 added rotCov()
 ***************************************************************************/

#ifndef QORC_COV2D_H
#define QORC_COV2D_H

namespace orcaqgui {

/*!
  \author Alex Makarenko
*/
class Cov2d
{
public:
  Cov2d (double xx=0.0, double xy=0.0, double yy=0.0) :
  		xx_(xx), xy_(xy), yy_(yy) {}

  double xx() const;
  double xy() const;
  double yy() const;
  void setCov( const double & xx, const double & xy, const double & yy )
  { xx_=xx; xy_=xy; yy_=yy; };

  Cov2d inverse() const;
  double det() const;
  void eigval( double &, double & ) const;
  void eigvect( const double &, const double &, double &, double &, double &, double & ) const;

  // rotate ellipse into principle axes
  // returns two standard deviations (A,B) in prinicple axes,
  // and rotation angle TH from physical CS into principle
  void ellipse( double & a, double & b, double & th ) const;

  // similar to above, not maintained
  //void ellipse( std::vector<double> &x, std::vector<double> &y, int numPoints ) const;

  // returns 1-D position uncertainty SR in a given direction, e.g. for a single laser return
  // requires rotation into principle CS first, so more efficient to do all
  // bearing angles BEARING [rad] at once
  double rotCov( const double & bearing ) const;
  double rotCov( const double & bearing, const double & s1, const double & s2, const double & th ) const;
  // similar but handles vectors internally, not maintained
  //void rotCov( std::vector<double> & sr, const std::vector<double> & bearing );
  
private:
	double xx_, xy_, yy_;
};


// access
inline double Cov2d::Cov2d::xx() const
{ return xx_; }

inline double Cov2d::Cov2d::xy() const
{ return xy_; }

inline double Cov2d::Cov2d::yy() const
{ return yy_; }

inline double Cov2d::det() const
{ return xx_*yy_ - xy_*xy_; }


}
#endif
