#ifndef ORCANAVUTIL_POSE_H
#define ORCANAVUTIL_POSE_H

#include <cmath>
#include <iostream>
#include <string>
#include <assert.h>
#include <orcanavutil/offset.h>

namespace orcanavutil {

//!
//! @author Alex Brooks
//!
class Pose
{

public: 

    Pose() {};
    Pose( double x, double y, double theta )
        :x_(x),y_(y),theta_(theta) {}

    // non-const versions
    double &x()     { return x_; }
    double &y()     { return y_; }
    double &theta() { return theta_; }
    
    // const versions
    double x()     const { return x_; }
    double y()     const { return y_; }
    double theta() const { return theta_; }

    // Access like a vector
    double v( int i ) const
        { 
            if      ( i==0 ) return x_;
            else if ( i==1 ) return y_;
            else if ( i==2 ) return theta_;
            else { assert( false&&"bad index" ); return 0; }
        }

    std::string toString() const;

private: 

    double x_, y_, theta_;

};
std::ostream &operator<<( std::ostream &s, const Pose &p );

//
// Pose manipulation convenience functions (see offset.h)
//

inline void addPoseOffset( Pose       &pose,
                           const Pose &offset,
                           bool        normaliseHeading )
{
    addPoseOffset( pose.x(), pose.y(), pose.theta(),
                   offset.x(), offset.y(), offset.theta(),
                   normaliseHeading );
}

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

inline void subtractInitialOffset( Pose       &pose,
                                   const Pose &initialOffset )
{
    subtractInitialOffset( pose.x(), pose.y(), pose.theta(),
                           initialOffset.x(), initialOffset.y(), initialOffset.theta() );
}

inline void subtractInitialOffset( const Pose &pose,
                                   const Pose &initialOffset,
                                   Pose       &result )
{
    subtractInitialOffset( pose.x(), pose.y(), pose.theta(),
                           initialOffset.x(), initialOffset.y(), initialOffset.theta(),
                           result.x(), result.y(), result.theta() );
}



}

#endif
