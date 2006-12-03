#include "rand.h"
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////

namespace orcamisc {

float
normalRand( float mean, float std ) 
{
   float x1, x2, w;
   do
   {
      x1 = 2.0f * (float)rand( )/RAND_MAX - 1.0f;
      x2 = 2.0f * (float)rand( )/RAND_MAX - 1.0f;
      w = x1*x1 + x2*x2;
   }
   while (w >= 1.0f || w < 1E-30);
   w = sqrt((-2.0f*log(w))/w);
   x1 *= w;
   // x2 *= w;  // a second normally distributed result not used
   return x1 * std + mean;
}

float 
randNum( float minVal, float maxVal )
{
    assert( maxVal > minVal );

    // RAND_MAX/(RAND_MAX+1) == 1.0 due to numerical accuracy.
    // Make sure this doesn't happen.
    while ( true )
    {
        float range = maxVal-minVal;
        float r = (float)((rand() / (RAND_MAX + 1.0)));

        float ret = (r*range) + minVal;
        if ( ret != maxVal )
            return ret;
    }
}

float 
randNumInclusive( float minVal, float maxVal )
{
    assert( maxVal >= minVal );
    float range = maxVal-minVal;
    float r = (float)((rand() / ((float)RAND_MAX)));

    return (r*range) + minVal;
}

}
