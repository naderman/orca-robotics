/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCAMISC_NRAND__H
#define ORCAMISC_NRAND__H

#include <cmath>

//
// General-purpose random number utilities
//
// Author: Alex Brooks
//

namespace orcamisc {

    // Fuck you, windows.
    // const double INV_SQRT_TWO_PI = 1/sqrt(2*M_PI);
    const double INV_SQRT_TWO_PI = (double)(1.0/std::sqrt(2*3.14159265358979));

    //! Returns the height of a 1D gaussian at 'dist' from the mean
    inline double G( double dist, double sd )
    { return (double)( (INV_SQRT_TWO_PI/sd) * std::exp( -(dist*dist)/(2.0*sd*sd) ) ); }

    //! Returns the height of a 1D gaussian at 'dist' from the mean
    inline double GCov( double dist, double cov )
    { return (double)( (INV_SQRT_TWO_PI/std::sqrt(cov)) * std::exp( -(dist*dist)/(2.0*cov) ) ); }

    //! The value of the two-dimensional axis-aligned gaussian with covariance xx,xy,yy,
    //! at the point 'diffs'.
    inline double multiG( double xDiff, double yDiff, double xx, double yy )
    { return G(xDiff,std::sqrt(xx)) * G(yDiff,std::sqrt(yy)); }

    //! Normally-distributed random number
    double normalRand( double mean, double std );

    //! returns a random number in the range [minVal,maxVal)
    double randNum( double minVal, double maxVal );

    //! returns a random number in the range [minVal,maxVal]
    double randNumInclusive( double minVal, double maxVal );
}

#endif
