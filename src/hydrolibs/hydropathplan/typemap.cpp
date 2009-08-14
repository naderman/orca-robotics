#include "typemap.h"
#include <iostream>
#include <cmath>

using namespace std;

namespace hydropathplan {

float element( const FloatMap &floatMap, int x, int y )
{ 
    float val;
    bool isValidElement = floatMap.tryElement(x,y,val);
    if (isValidElement)
    {
        return val;
    }
    else
    {
        return NAN;
    }
}

}

