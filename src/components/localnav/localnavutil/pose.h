#ifndef POSE_H
#define POSE_H

#include <cmath>
#include <orcageom2d/geom2d.h>
#include <orcanavutil/pose.h>
#include <orca/localise2d.h>


namespace localnav {

inline double distance( const orcanavutil::Pose &pose, const geom2d::Point &point )
{ return hypotf( pose.y()-point.y(), pose.x()-point.x() ); }
inline double distance( const geom2d::Point &point, const orcanavutil::Pose &pose )
{ return distance( pose, point ); }

orcanavutil::Pose getMLPose( const orca::Localise2dData &localiseData );


}

#endif
