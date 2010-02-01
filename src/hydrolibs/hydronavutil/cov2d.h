#ifndef HYDRONAVUTIL_COV2D_H
#define HYDRONAVUTIL_COV2D_H

#include <iostream>
#include <assert.h>

namespace hydronavutil {

//!
//! @brief 2d covariance matrix
//!
//! @author Alex Brooks
//!
class Cov2d
{

public: 

    Cov2d( double xx, double xy, double yy )
        : xx_(xx),xy_(xy),yy_(yy) {}

    double xx() const { return xx_; }
    double xy() const { return xy_; }
    double yy() const { return yy_; }

    double &xx() { return xx_; }
    double &xy() { return xy_; }
    double &yy() { return yy_; }

    // access like a matrix
    double m( int i, int j ) const
        {
            if      ( i==0 && j==0 ) return xx_;
            else if ( i==0 && j==1 ) return xy_;
            else if ( i==1 && j==0 ) return xy_;
            else if ( i==1 && j==1 ) return yy_;
            else { assert( false&&"bad index" ); return 0; }
        }

private: 

    double xx_, xy_, yy_;

};
std::ostream &operator<<( std::ostream &s, const Cov2d &c );

}

#endif
