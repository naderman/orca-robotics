#ifndef ODOMETRYDIFFERENTIATOR_H
#define ODOMETRYDIFFERENTIATOR_H

#include <orcanavutil/orcanavutil.h>

namespace orcanavutil {

//!
//! @brief Feed in odometry, this will tell you what change each new bit of data represents.
//!
//! @author Alex Brooks
//!
class OdometryDifferentiator
{

public: 

    OdometryDifferentiator()
        : prevOdomInitialised_(false)
        {}

    //! Calculates the delta that this new odometry reading represents
    void addOdom( double x,
                  double y,
                  double theta,
                  orcanavutil::Offset &delta );

private: 

    double prevX_, prevY_, prevTheta_;
    bool prevOdomInitialised_;
};

}

#endif
