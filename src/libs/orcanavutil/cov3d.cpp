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
Cov3d::det()
{
    double ytSq = yt()*yt();
    double xySq = xy()*xy();
    double xtSq = xt()*xt();

    return xx()*yy()*tt() - xx()*ytSq - xySq*tt() + 2*xy()*xt()*yt() - xtSq*yy();    
}

Cov3d
Cov3d::inverse()
{
    double d = det();

    // [   (yy*tt-yt^2)/d, -(xy*tt-xt*yt)/d,  (xy*yt-xt*yy)/d]
    // [ -(xy*tt-xt*yt)/d,   (xx*tt-xt^2)/d, -(xx*yt-xy*xt)/d]
    // [  (xy*yt-xt*yy)/d, -(xx*yt-xy*xt)/d,   (xx*yy-xy^2)/d]

    double ytSq = yt()*yt();
    double xySq = xy()*xy();
    double xtSq = xt()*xt();

    return Cov3d( (yy()*tt()-ytSq)/d,
                  -(xy()*tt()-xt()*yt())/d,
                  (xy()*yt()-xt()*yy())/d,
                  (xx()*tt()-xtSq)/d,
                  -(xx()*yt()-xy()*xt())/d,
                  (xx()*yy()-xySq)/d );
}

double
Cov3d::gauss( double x, double y, double t )
{
    Cov3d i = inverse();

    double expBit = 
        (x*i.xx()+y*i.xy()+t*i.xt())*x +
        (x*i.xy()+y*i.yy()+t*i.yt())*y +
        (x*i.xt()+y*i.yt()+t*i.tt())*t;

    return CONSTANT_BIT * 1.0/sqrt(det()) * exp( -0.5 * expBit );
}

}
