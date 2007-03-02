#ifndef POSE_H
#define POSE_H

#include <orcageom2d/geom2d.h>
#include <cmath>
#include <iostream>
#include <string>

// for getMLPose()
#include <orcanavutil/pose.h>
#include <orca/localise2d.h>


namespace localnav {

//!
//! @author 
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

    std::string toString() const;

private: 

    double x_, y_, theta_;

};

inline double distance( const Pose &pose, const geom2d::Point &point )
{ return hypotf( pose.y()-point.y(), pose.x()-point.x() ); }
inline double distance( const geom2d::Point &point, const Pose &pose )
{ return distance( pose, point ); }

std::ostream &operator<<( std::ostream &s, const Pose &p );

orcanavutil::Pose getMLPose( const orca::Localise2dData &localiseData );


}

#endif
