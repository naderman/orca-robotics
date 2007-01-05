/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAOBJ_INIT_UTILS_H
#define ORCAOBJ_INIT_UTILS_H

#include <orca/orca.h>
#include <orca/camera.h>
#include <orca/drivebicycle.h>
#include <orca/gps.h>
#include <orca/ogmap.h>
#include <orca/home.h>
#include <orca/laserscanner2d.h>
#include <orca/odometry2d.h>
#include <orca/position2d.h>
#include <orca/position3d.h>
#include <orca/localise2d.h>
#include <orca/platform2d.h>
#include <orca/power.h>
#include <orca/status.h>
#include <orca/polarfeature2d.h>
#include <orca/featuremap2d.h>
#include <orca/pathfollower2d.h>
#include <orca/pathplanner2d.h>

namespace orcaice
{
/*!
 *  @name Object Initialization Functions
 */
//@{
//! Sets all fields to zero.
void setInit( orca::Frame2d& );
//! Sets all fields to zero.
void setInit( orca::Frame3d& );
//! Sets all fields to zero.
void setInit( orca::Twist2d& );
//! Sets all fields to zero.
void setInit( orca::Size2d& );
//! Sets all fields to zero.
void setInit( orca::Size3d& );
//! Sets all fields to zero.
void setInit( orca::CartesianPoint& );
//! Sets all fields to zero.
void setInit( orca::Time& );
//! Sets all fields to zero and stalled to FALSE.
void setInit( orca::Position2dData& );
//! Sets all fields to zero
void setInit( orca::Waypoint2d& );

//! Set to randomized but sane values.
void setSane( orca::CartesianPoint2d& );
//! Set to randomized but sane values.
void setSane( orca::CartesianPoint& );
//! Set to randomized but sane values.
void setSane( orca::CartesianVelocity2d& );
//! Set to randomized but sane values.
void setSane( orca::CartesianVelocity& );
//! Set to randomized but sane values.
void setSane( orca::AngularVelocity& );
//! Sets to current time.
void setSane( orca::Date& );
//! Set to randomized but sane values.
void setSane( orca::Frame2d& );
//! Set to randomized but sane values.
void setSane( orca::Frame3d& );
//! Set to randomized but sane values.
void setSane( orca::Size2d& );
//! Set to randomized but sane values.
void setSane( orca::Size3d& );
//! Sets to current time.
void setSane( orca::Time& );
//! Sets to current time.
void setSane( orca::TimeOfDay& );

//! Set to randomized but sane values.
void setSane( orca::CameraData&, int width=640, int height=480 );
//! Set to randomized but sane values.
void setSane( orca::DriveBicycleData& );
//! Set to randomized but sane values.
void setSane( orca::GpsData& );
//! Set to randomized but sane values.
void setSane( orca::GpsTimeData& );
//! Set to randomized but sane values.
void setSane( orca::GpsMapGridData& );
//! Set to randomized but sane values.
void setSane( const orca::RangeScanner2dDataPtr&, int numberOfSamples=361 );
//! Set to randomized but sane values.
void setSane( const orca::LaserScanner2dDataPtr&, int numberOfSamples=361 );
//! Set to randomized but sane values.
void setSane( orca::Odometry2dData& );
//! Set to randomized but sane values.
void setSane( orca::OgMapData&, int width=640, int height=480 );
//! Set to randomized but sane values.
void setSane( orca::Position2dData& );
//! Set to randomized but sane values.
void setSane( orca::Position3dData& );
//! Set to randomized but sane values.
void setSane( orca::PowerData&, int count=2 );
//! Set to randomized but sane values.
void setSane( orca::Velocity2dCommand& );
//@}

//@}


} // namespace

#endif
