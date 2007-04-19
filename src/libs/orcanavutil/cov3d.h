#ifndef ORCANAVUTIL_COV3D_H
#define ORCANAVUTIL_COV3D_H

#include <iostream>

namespace orcanavutil {

//!
//! @brief 3d covariance matrix
//!
//! @author Alex Brooks
//!
class Cov3d
{
private: 

    static const int PXX   = 0;
    static const int PXY   = 1;
    static const int PYY   = 2;

    static const int PXT   = 3;
    static const int PYT   = 4;
    static const int PTT   = 5;

public: 

    Cov3d( double xx, 
           double xy, 
           double xt,
           double yy,
           double yt,
           double tt )
        {
            mat_[PXX] = xx;
            mat_[PYY] = yy;
            mat_[PTT] = tt;
            mat_[PXY] = xy;
            mat_[PXT] = xt;
            mat_[PYT] = yt;
        }

    double &xx()   { return mat_[PXX]; }
    double &yy()   { return mat_[PYY]; }
    double &tt()   { return mat_[PTT]; }
    double &xy()   { return mat_[PXY]; }
    double &xt()   { return mat_[PXT]; }
    double &yt()   { return mat_[PYT]; }

    double xx() const   { return mat_[PXX]; }
    double yy() const   { return mat_[PYY]; }
    double tt() const   { return mat_[PTT]; }
    double xy() const   { return mat_[PXY]; }
    double xt() const   { return mat_[PXT]; }
    double yt() const   { return mat_[PYT]; }

    //! determinant
    double det();
    Cov3d inverse();

    //! Height of a gaussian with this covariance matrix, centred on (0,0,0),
    //! evaluated at the point (x,y,t)
    double gauss( double x, double y, double t );

private: 

    double mat_[6];
};

std::ostream &operator<<( std::ostream &s, const Cov3d &c );

}

#endif
