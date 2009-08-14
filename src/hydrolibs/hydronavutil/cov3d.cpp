#include "cov3d.h"
#include <iostream>
#include <cmath>
#include <sstream>
#include <hydroportability/windows.h>

using namespace std;

namespace hydronavutil {

namespace {
    static const double CONSTANT_BIT = std::pow( (2*M_PI), -3.0/2.0 );
}

std::ostream &operator<<( std::ostream &s,
                          const Cov3d &c )
{
    s << endl
      << "[ " << c.xx() << ",\t" << c.xy() << ",\t" << c.xt() << endl
      << "  " << "\t\t" << c.yy() << ",\t" << c.yt() << endl
      << "  " << "\t\t\t" << c.tt() << " ]";
    return s;
}

std::string toMatlabString( const Cov3d &c )
{
    stringstream s;
    s << endl
      << "[ " << c.xx() << ",\t" << c.xy() << ",\t" << c.xt() << "; ..." << endl
      << "  " << c.xy() << ",\t" << c.yy() << ",\t" << c.yt() << "; ..." << endl
      << "  " << c.xt() << ",\t" << c.yt() << ",\t" << c.tt() << " ]";
    return s.str();
}

double 
Cov3d::det() const
{
    return hydronavutil::det( xx(), xy(), xt(), yy(), yt(), tt() );
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

    double ret = CONSTANT_BIT * 1.0/sqrt(det()) * exp( -0.5 * expBit );
    if ( isnan(ret) ||
         isinf(ret) )
    {
        stringstream ss;
        ss << "Cov3d::gauss(): numerical error: guass("<<x<<", "<<y<<", "<<t<<") returned " << ret << "." << endl
           << "  Covariance is: " << *this << endl
           << "  det() is: " << det() << endl
           << "  inverse is: " << inverse();
        throw Exception( ERROR_INFO, ss.str() );
    }    
    return ret;
}

}
