#ifndef POSE_H
#define POSE_H

#include <cmath>
#include <orcageom2d/geom2d.h>
#include <hydronavutil/pose.h>
#include <orca/localise2d.h>


namespace localnav {

inline double distance( const hydronavutil::Pose &pose, const geom2d::Point &point )
{ return hypotf( pose.y()-point.y(), pose.x()-point.x() ); }
inline double distance( const geom2d::Point &point, const hydronavutil::Pose &pose )
{ return distance( pose, point ); }

hydronavutil::Pose getMLPose( const orca::Localise2dData &localiseData );


}

#endif
