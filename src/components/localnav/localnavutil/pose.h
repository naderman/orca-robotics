/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef POSE_H
#define POSE_H

#include <cmath>
#include <hydrogeom2d/geom2d.h>
#include <hydronavutil/pose.h>
#include <orca/localise2d.h>


namespace localnav {

inline double distance( const hydronavutil::Pose &pose, const geom2d::Point &point )
{ return hypot( pose.y()-point.y(), pose.x()-point.x() ); }

inline double distance( const geom2d::Point &point, const hydronavutil::Pose &pose )
{ return distance( pose, point ); }

hydronavutil::Pose getMLPose( const orca::Localise2dData &localiseData );


}

#endif
