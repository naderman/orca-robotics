#ifndef ORCANAVUTIL_COV3D_H
#define ORCANAVUTIL_COV3D_H

#include <cstdlib>
#include <iostream>
#include <assert.h>
#include <hydronavutil/exceptions.h>

namespace hydronavutil {

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

    Cov3d() {}

    //! Construct arbitrary covariance matrices
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

    //! Convenience function for constructing diagonal matrices
    Cov3d( double xx, 
           double yy,
           double tt )
        {
            mat_[PXX] = xx;
            mat_[PXY] = 0;
            mat_[PXT] = 0;
            mat_[PYY] = yy;
            mat_[PYT] = 0;
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
    double det() const;
    Cov3d inverse() const;

    //! Height of a gaussian with this covariance matrix, centred on (0,0,0),
    //! evaluated at the point (x,y,t)
    double gauss( double x, double y, double t ) const;

    //! access like a matrix
    double m( int i, int j ) const
        {
            if      ( i==0 && j==0 ) return xx();
            else if ( ( i==0 && j==1 ) || ( i==1 && j==0 ) ) return xy();
            else if ( ( i==0 && j==2 ) || ( i==2 && j==0 ) ) return xt();
            else if ( i==1 && j==1 ) return yy();
            else if ( ( i==1 && j==2 ) || ( i==2 && j==1 ) ) return yt();
            else if ( i==2 && j==2 ) return tt();
            else { assert( false&&"bad index" ); return 0; }
        }
    //! access like a matrix
    double &m( int i, int j )
        {
            if      ( i==0 && j==0 ) return xx();
            else if ( ( i==0 && j==1 ) || ( i==1 && j==0 ) ) return xy();
            else if ( ( i==0 && j==2 ) || ( i==2 && j==0 ) ) return xt();
            else if ( i==1 && j==1 ) return yy();
            else if ( ( i==1 && j==2 ) || ( i==2 && j==1 ) ) return yt();
            else if ( i==2 && j==2 ) return tt();
            else { assert( false&&"bad index" ); exit(1); }
        }

    bool isSPD() const
        { return det() > 0; }

private: 

    double mat_[6];
};

// Prints out in a slightly more sparse format which is easier to read on the screen
std::ostream &operator<<( std::ostream &s, const Cov3d &c );

// Returns a string formatted such that it can be cut-n-pasted into matlab
std::string toMatlabString( const Cov3d &c );

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

    if ( d == 0 )
        throw hydronavutil::Exception( ERROR_INFO, "found zero determinant during attempt to invert" );

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

// Allow use outside of class.
//! Calculates the height of a gaussian with this covariance matrix, centred on (0,0,0),
//! evaluated at the point (x,y,t)
inline double gauss( double xx,
                     double xy,
                     double xt,
                     double yy,
                     double yt,
                     double tt,
                     double x,
                     double y,
                     double t )
{
    Cov3d c( xx, xy, xt, yy, yt, tt );
    return c.gauss( x, y, t );
}

}

#endif
