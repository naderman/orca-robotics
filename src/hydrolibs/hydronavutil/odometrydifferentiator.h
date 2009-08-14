#ifndef ODOMETRYDIFFERENTIATOR_H
#define ODOMETRYDIFFERENTIATOR_H

#include <hydronavutil/hydronavutil.h>

namespace hydronavutil {

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

    OdometryDifferentiator( const hydronavutil::Pose &initPose );

    //! Returns the delta that this new odometry reading represents
    hydronavutil::Pose calcDelta( double x,
                                  double y,
                                  double theta );

    //! Returns the delta that this new odometry reading represents
    hydronavutil::Pose calcDelta( const hydronavutil::Pose &newPose )
        { return calcDelta( newPose.x(), newPose.y(), newPose.theta() ); }

private: 

    double prevX_, prevY_, prevTheta_;
    bool prevOdomInitialised_;
};

}

#endif
