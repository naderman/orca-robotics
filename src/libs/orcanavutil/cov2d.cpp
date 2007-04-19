#include "cov2d.h"
#include <iostream>

using namespace std;

namespace orcanavutil {

std::ostream &operator<<( std::ostream &s,
                          const Cov2d &c )
{
    return s << "["<<c.xx()<<", "<<c.xy()<<"; "<<c.xy()<<", "<<c.yy()<<"]";
}

}
