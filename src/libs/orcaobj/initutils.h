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
#include <orca/gps.h>
#include <orca/ogmap.h>
#include <orca/home.h>
#include <orca/laserscanner2d.h>
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
void setInit( orca::Frame2d & );
//! Sets all fields to zero.
void setInit( orca::Frame3d & );
//! Sets all fields to zero.
void setInit( orca::Size2d & );
//! Sets all fields to zero.
void setInit( orca::Size3d & );
//! Sets all fields to zero.
void setInit( orca::CartesianPoint & );
//! Sets all fields to zero.
void setInit( orca::Time & );
//! Sets all fields to zero and stalled to FALSE.
void setInit( const orca::Position2dDataPtr & );
//! Sets all fields to zero
void setInit( orca::Waypoint2d & );

//! Set to randomized but sane values.
void setSane( orca::CartesianPoint2d & );
//! Set to randomized but sane values.
void setSane( orca::CartesianPoint & );
//! Sets to current time.
void setSane( orca::Date & );
//! Set to randomized but sane values.
void setSane( orca::Frame2d & );
//! Set to randomized but sane values.
void setSane( orca::Frame3d & );
//! Set to randomized but sane values.
void setSane( orca::Size2d & );
//! Set to randomized but sane values.
void setSane( orca::Size3d & );
//! Sets to current time.
void setSane( orca::Time & );
//! Sets to current time.
void setSane( orca::TimeOfDay & );

//! Set to randomized but sane values.
void setSane( const orca::CameraDataPtr &, int width=640, int height=480 );
//! Set to randomized but sane values.
void setSane( const orca::GpsDataPtr & );
//! Set to randomized but sane values.
void setSane( const orca::GpsTimeDataPtr & );
//! Set to randomized but sane values.
void setSane( const orca::GpsMapGridDataPtr & );
//! Set to randomized but sane values.
void setSane( const orca::LaserScanner2dDataPtr &, int numberOfSamples=361 );
//! Set to randomized but sane values.
void setSane( const orca::OgMapDataPtr &, int width=640, int height=480 );
//! Set to randomized but sane values.
void setSane( const orca::Position2dDataPtr & );
//! Set to randomized but sane values.
void setSane( const orca::Position3dDataPtr & );
//! Set to randomized but sane values.
void setSane( const orca::PowerDataPtr &, int count=2 );
//! Set to randomized but sane values.
void setSane( const orca::Velocity2dCommandPtr & );
//@}

//@}


} // namespace

#endif
