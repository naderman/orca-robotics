#include "brosutil.h"
#include <iostream>

using namespace std;

namespace orcalocalnavutil {

    void setToZero( orca::Frame3d &f )
    {
        f.p.x = 0;
        f.p.y = 0;
        f.p.z = 0;
        f.o.r = 0;
        f.o.p = 0;
        f.o.y = 0;
    }

    bool isZero( const orca::Frame3d &f )
    {
        return ( f.p.x == 0 &&
                 f.p.y == 0 &&
                 f.p.z == 0 &&
                 f.o.r == 0 &&
                 f.o.p == 0 &&
                 f.o.y == 0 );
    }

}
