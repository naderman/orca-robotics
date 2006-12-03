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
        : prevOdomInitialised_(false),
          prevOdom_(new orca::Position2dData)
        {}

    //! Calculates the delta that this new odometry reading represents
    void addOdom( const orca::Position2dDataPtr odom, orcanavutil::Offset &delta );

private: 

    bool                    prevOdomInitialised_;
    orca::Position2dDataPtr prevOdom_;

};

}

#endif
