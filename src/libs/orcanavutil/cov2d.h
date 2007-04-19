#ifndef ORCANAVUTIL_COV2D_H
#define ORCANAVUTIL_COV2D_H

#include <iostream>

namespace orcanavutil {

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

private: 

    double xx_, xy_, yy_;

};

std::ostream &operator<<( std::ostream &s, const Cov2d &c );

}

#endif
