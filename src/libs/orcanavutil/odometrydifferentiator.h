#ifndef ODOMETRYDIFFERENTIATOR_H
#define ODOMETRYDIFFERENTIATOR_H

#include <orca/odometry2d.h>
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
    void addOdom( const orca::Odometry2dData& odom, orcanavutil::Offset &delta );

private: 

    bool                 prevOdomInitialised_;
    orca::Odometry2dData prevOdom_;
};

}

#endif
