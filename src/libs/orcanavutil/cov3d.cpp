#include "cov3d.h"
#include <iostream>
#include <cmath>

using namespace std;

namespace orcanavutil {

namespace {
    static const double CONSTANT_BIT = std::pow( (2*M_PI), -3.0/2.0 );
}

std::ostream &operator<<( std::ostream &s,
                          const Cov3d &c )
{
    s << "[ " << c.xx() << ",\t" << c.xy() << ",\t" << c.xt() << endl
      << "  " << "\t\t" << c.yy() << ",\t" << c.yt() << endl
      << "  " << "\t\t\t" << c.tt() << " ]";
    return s;
}

double 
Cov3d::det() const
{
    return orcanavutil::det( xx(), xy(), xt(), yy(), yt(), tt() );
}

Cov3d
Cov3d::inverse() const
{
    double ixx, ixy, ixt, iyy, iyt, itt;
    invert( xx(), xy(), xt(), yy(), yt(), tt(),
            ixx,  ixy,  ixt,  iyy,  iyt,  itt );

    return Cov3d( ixx,  ixy,  ixt,  iyy,  iyt,  itt );
}

double
Cov3d::gauss( double x, double y, double t ) const
{
    Cov3d i = inverse();

    double expBit = 
        (x*i.xx()+y*i.xy()+t*i.xt())*x +
        (x*i.xy()+y*i.yy()+t*i.yt())*y +
        (x*i.xt()+y*i.yt()+t*i.tt())*t;

    return CONSTANT_BIT * 1.0/sqrt(det()) * exp( -0.5 * expBit );
}

}
