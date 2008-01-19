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
                          cov2d.cpp  -  description
                             -------------------
    begin                : Thu Jun 27 2002
    copyright            : (C) 2002 by am
    email                : a.makarenko@acfr.usyd.edu.au
    description          : 2D covariance matrix
                         :
    versions             : v0 based Tim Baley's code
                         : v1 01/11/02 fixed version of ellipse with a,b,th
                         : v2 05/01/03 added rotCov()
 ***************************************************************************/

#include <cmath>

#include "cov2d.h"

using namespace std;

namespace laser2og {

void Cov2d::ellipse( double & a, double & b, double & th ) const
{
  // eigenvalues
  double d1,d2;
  eigval( d1,d2 );
  // standard deviations in principle CS
  a = sqrt(d1);
  b = sqrt(d2);
  
  // eigenvectors [ [v1x v1y]' [v2x v2y]' ]
  double v1x,v1y, v2x, v2y;
  eigvect( d1,d2, v1x,v1y, v2x, v2y );

  // th = atan2( 2.0*xy_, xx_+yy_ ); // is this true? where does it come from?
  th = atan2( v1y, v1x );
}

Cov2d Cov2d::inverse() const
{
	double D = det();
	double a = yy_/D;
	double b = -xy_/D;
	double c = xx_/D;

   return Cov2d( a, b, c );
}

void Cov2d::eigval( double & d1, double & d2 ) const
{
  double lamcom1 = yy_ + xx_;
	double lamcom2 = sqrt((yy_+xx_)*(yy_+xx_) - 4.0*det());
	d1 = ( lamcom1 + lamcom2 ) / 2.0;
	d2 = ( lamcom1 - lamcom2 ) / 2.0;
}

void Cov2d::eigvect( const double & d1, const double & d2, double & v1x, double & v1y, double & v2x, double & v2y ) const
{
	// compute the first eigen vector
  v1x = d1 + xy_ - yy_;
  v1y = d1 + xy_ - xx_;
  double n1 = sqrt( v1x*v1x + v1y*v1y );

  // compute the second eigen vector
  v2x = d2 + xy_ - yy_;
  v2y = d2 + xy_ - xx_;
  double n2 = sqrt( v2x*v2x + v2y*v2y );

  // normalise
  v1x /= n1;
  v1y /= n1;

  v2x /= n2;
  v2y /= n2;
}

double Cov2d::rotCov( const double & bearing ) const
{
	double s1, s2, th;
	ellipse( s1, s2, th );

	return rotCov( bearing, s1, s2, th );
}

/**
 * eigenvalue problem doesn't have to be solved every time if principle axis components are
 * given
 */
double Cov2d::rotCov( const double & bearing, const double & s1, const double & s2, const double & th ) const
{
	// the ratio of principle st.dev's squared : (sigma2/sigma1)^2
	double s21_2 = pow( s2/s1, 2.0 );

	// the line coeff. to the target squared : k^2
	double k2 = pow( tan( bearing - th ), 2.0 );

	double srot = s2 * sqrt( (k2 + 1.0) / ( k2 + s21_2 ) );

	return srot;
}


/**
 *  The solution is the distance from the center of the covariance ellipse to the
 *  intersection between the ellipse and a ray whose angle in global CS is bearing
 *  to the target.
 */
/*
void Cov2d::rotCov( std::vector<double> & sr, const vector<double> & bearing )
{
	double s1, s2, th;
	ellipse( s1, s2, th );

	// the ratio of principle st.dev's squared : (sigma2/sigma1)^2
	double s21_2 = pow( s2/s1, 2.0 );

	// the line coeff. to the target squared : k^2
  double k2 = 0.0;

	sr.clear();
	for ( unsigned int i=0; i<bearing.size(); ++i ) {
		k2 = pow( tan( bearing[i] - th ), 2.0 );
		sr.push_back( s2 * sqrt( (k2 + 1.0) / ( k2 + s21_2 ) ) );
	}
}
*/
// keep for a bit
/*
void Cov2d::ellipse( std::vector<double> &x, std::vector<double> &y, int numPoints ) const
{
  // eigenvalues
  double d1,d2;
  eigval( d1,d2 );
  // standard deviations in principle CS
  const double s1 = sqrt(d1);
  const double s2 = sqrt(d2);

  // eigenvectors [ [v1x v1y]' [v2x v2y]' ]
  double v1x,v1y, v2x, v2y;
  eigvect( d1,d2, v1x,v1y, v2x, v2y );

  double da = 2.0*angled::Pi/numPoints;
  double angle = 0;

  double y1,y2, x1,x2;
  for (int i = 0; i < numPoints; i++)
  {
    // ellipse points in principle CS
    y1 = s1 * std::cos(angle);
    y2 = s2 * std::sin(angle);

    // ellipse points in physical CS: x = Q'y (notice transpose!)
    x1 = v1x*y1 + v1y*y2;
    x2 = v2x*y1 + v2y*y2;

    x.push_back(x1);
    y.push_back(x2);

    angle += da;
  }
}
*/

}
