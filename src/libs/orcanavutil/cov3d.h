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
    static const int PXT   = 2;

    static const int PYY   = 3;
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
            mat_[PXY] = xy;
            mat_[PXT] = xt;
            mat_[PYY] = yy;
            mat_[PYT] = yt;
            mat_[PTT] = tt;
        }

    double &xx()   { return mat_[PXX]; }
    double &xy()   { return mat_[PXY]; }
    double &xt()   { return mat_[PXT]; }
    double &yy()   { return mat_[PYY]; }
    double &yt()   { return mat_[PYT]; }
    double &tt()   { return mat_[PTT]; }

    double xx() const   { return mat_[PXX]; }
    double xy() const   { return mat_[PXY]; }
    double xt() const   { return mat_[PXT]; }
    double yy() const   { return mat_[PYY]; }
    double yt() const   { return mat_[PYT]; }
    double tt() const   { return mat_[PTT]; }

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

// Allow use outside of class
inline double det( double xx,
                   double xy,
                   double xt,
                   double yy,
                   double yt,
                   double tt )
{
    double ytSq = yt*yt;
    double xySq = xy*xy;
    double xtSq = xt*xt;
    return xx*yy*tt - xx*ytSq - xySq*tt + 2*xy*xt*yt - xtSq*yy;    
}         

// Allow use outside of class
inline void invert( double xx,
                    double xy,
                    double xt,
                    double yy,
                    double yt,
                    double tt,
                    double &xxOut,
                    double &xyOut,
                    double &xtOut,
                    double &yyOut,
                    double &ytOut,
                    double &ttOut )
{
    double d = det( xx, xy, xt, yy, yt, tt );

    double ytSq = yt*yt;
    double xySq = xy*xy;
    double xtSq = xt*xt;

    xxOut = (yy*tt-ytSq)/d;
    xyOut = -(xy*tt-xt*yt)/d;
    xtOut = (xy*yt-xt*yy)/d;
    yyOut = (xx*tt-xtSq)/d;
    ytOut = -(xx*yt-xy*xt)/d;
    ttOut = (xx*yy-xySq)/d;
}

}

#endif
