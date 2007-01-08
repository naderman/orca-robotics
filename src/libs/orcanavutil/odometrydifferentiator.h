#ifndef ODOMETRYDIFFERENTIATOR_H
#define ODOMETRYDIFFERENTIATOR_H

#include <orca/position2d.h>
#include <orca/odometry2d.h>
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
    void addOdom( const orca::Odometry2dData& odom, orcanavutil::Offset &delta );
    
    //! As above. Obsolete, use the above instead. Provided for backwards compatibility.
    void addOdom( const orca::Position2dData& odom, orcanavutil::Offset &delta );

private: 

    bool                 prevOdomInitialised_;
    orca::Odometry2dData prevOdom_;
    orca::Position2dData prevPos_;

};

}

#endif
