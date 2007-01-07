/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "rand.h"
#include <cmath>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <sstream>

using namespace std; // for math functions in QNX

////////////////////////////////////////////////////////////////////////////////

namespace orcamisc {

double
normalRand( double mean, double std ) 
{
   double x1, x2, w;
   do
   {
      x1 = 2.0f * (double)rand( )/RAND_MAX - 1.0f;
      x2 = 2.0f * (double)rand( )/RAND_MAX - 1.0f;
      w = x1*x1 + x2*x2;
   }
   while (w >= 1.0f || w < 1E-30);
   w = sqrt((-2.0f*log(w))/w);
   x1 *= w;
   // x2 *= w;  // a second normally distributed result not used
   return x1 * std + mean;
}

double
randNum( double minVal, double maxVal )
{
    assert( maxVal > minVal );
    if ( !(maxVal > minVal) )
    {
        std::stringstream ss;
        ss << "randNum: expected a range, got minVal="<<minVal<<", maxVal="<<maxVal;
        throw( ss.str() );
    }

    // RAND_MAX/(RAND_MAX+1) == 1.0 due to numerical accuracy.
    // Make sure this doesn't happen.
    while ( true )
    {
        double range = maxVal-minVal;
        double r = (rand() / (RAND_MAX + 1.0));

        double ret = (r*range) + minVal;
        if ( ret != maxVal )
            return ret;
    }
}

double 
randNumInclusive( double minVal, double maxVal )
{
    assert( maxVal >= minVal );
    double range = maxVal-minVal;
    double r = (rand() / ((double)RAND_MAX));

    return (r*range) + minVal;
}

}
