#ifndef ODOMETRYDIFFERENTIATOR_H
#define ODOMETRYDIFFERENTIATOR_H

#include <orca/position2d.h>
#include <orcanavutil/orcanavutil.h>

namespace orcanavutil {

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
    void addOdom( const orca::Position2dData& odom, orcanavutil::Offset &delta );

private: 

    bool                 prevOdomInitialised_;
    orca::Position2dData prevOdom_;

};

}

#endif
