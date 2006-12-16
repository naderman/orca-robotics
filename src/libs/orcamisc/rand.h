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
    // const float INV_SQRT_TWO_PI = 1/sqrt(2*M_PI);
    const float INV_SQRT_TWO_PI = (float)(1.0/std::sqrt(2*3.14159265358979));

    //! Returns the height of a 1D gaussian at 'dist' from the mean
    inline float G( float dist, float sd )
    { return (float)( (INV_SQRT_TWO_PI/sd) * std::exp( -(dist*dist)/(2.0*sd*sd) ) ); }

    //! Returns the height of a 1D gaussian at 'dist' from the mean
    inline float GCov( float dist, float cov )
    { return (float)( (INV_SQRT_TWO_PI/std::sqrt(cov)) * std::exp( -(dist*dist)/(2.0*cov) ) ); }

    //! The value of the two-dimensional axis-aligned gaussian with covariance xx,xy,yy,
    //! at the point 'diffs'.
    inline float multiG( float xDiff, float yDiff, float xx, float yy )
    { return G(xDiff,std::sqrt(xx)) * G(yDiff,std::sqrt(yy)); }

    //! Normally-distributed random number
    float normalRand( float mean, float std );

    //! returns a random number in the range [minVal,maxVal)
    float randNum( float minVal, float maxVal );

    //! returns a random number in the range [minVal,maxVal]
    float randNumInclusive( float minVal, float maxVal );
}

#endif
