#ifndef ORCANAVUTIL_POSE_H
#define ORCANAVUTIL_POSE_H

#include <cmath>
#include <iostream>
#include <string>
#include <assert.h>
#include <hydronavutil/offset.h>

namespace hydronavutil {

//!
//! @author Alex Brooks
//!
class Pose
{

public: 

    Pose() {};
    Pose( double x, double y, double theta )
        { p_[0]=x; p_[1]=y; p_[2]=theta; }

    // non-const versions
    double &x()     { return p_[0]; }
    double &y()     { return p_[1]; }
    double &theta() { return p_[2]; }
    
    // const versions
    double x()     const { return p_[0]; }
    double y()     const { return p_[1]; }
    double theta() const { return p_[2]; }

    // Access like a vector
    double *v() { return p_; }
    const double *v() const { return p_; }

    std::string toString() const;

private: 

    double p_[3];

};
std::ostream &operator<<( std::ostream &s, const Pose &p );

//////////////////////////////////////////////////////////////////////
//
// Non-member convenience functions for pose manipulation (see offset.h)
//

//! convenience
inline void normaliseHeading( Pose &pose )
{ NORMALISE_ANGLE( pose.theta() ); }

//! convenience
inline void addPoseOffset( Pose       &pose,
                           const Pose &offset,
                           bool        normaliseHeading )
{
    addPoseOffset( pose.x(), pose.y(), pose.theta(),
                   offset.x(), offset.y(), offset.theta(),
                   normaliseHeading );
}

//! convenience
inline void addPoseOffset( const Pose &start,
                           const Pose &offset,
                           Pose       &result,
                           bool        normaliseHeading )
{
    addPoseOffset( start.x(), start.y(), start.theta(),
                   offset.x(), offset.y(), offset.theta(),
                   result.x(), result.y(), result.theta(),
                   normaliseHeading );
}

//! convenience
inline void subtractFinalPoseOffset( Pose       &pose,
                                     const Pose &offset,
                                     bool        normaliseHeading )
{
    subtractFinalPoseOffset( pose.x(), pose.y(), pose.theta(),
                             offset.x(), offset.y(), offset.theta(),
                             normaliseHeading );
}

//! convenience
inline void subtractFinalPoseOffset( const Pose &start,
                                     const Pose &offset,
                                     Pose       &result,
                                     bool        normaliseHeading )
{
    subtractFinalPoseOffset( start.x(), start.y(), start.theta(),
                             offset.x(), offset.y(), offset.theta(),
                             result.x(), result.y(), result.theta(),
                             normaliseHeading );
}

//! convenience
inline void subtractInitialOffset( Pose       &pose,
                                   const Pose &initialOffset )
{
    subtractInitialOffset( pose.x(), pose.y(), pose.theta(),
                           initialOffset.x(), initialOffset.y(), initialOffset.theta() );
}

//! convenience
inline void subtractInitialOffset( const Pose &pose,
                                   const Pose &initialOffset,
                                   Pose       &result )
{
    subtractInitialOffset( pose.x(), pose.y(), pose.theta(),
                           initialOffset.x(), initialOffset.y(), initialOffset.theta(),
                           result.x(), result.y(), result.theta() );
}

//! convenience
inline Pose inverseTransform( const Pose &transform )
{
    hydronavutil::Pose inv;
    getInverseTransform( transform.x(), transform.y(), transform.theta(),
                         inv.x(), inv.y(), inv.theta() );
    return inv;
}

} // end namespace

#endif
