/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAOBJ_STRINGIFY_UTILITIES_H
#define ORCAOBJ_STRINGIFY_UTILITIES_H

#include <string>
#include <orca/ocm.h>
#include <orca/orca.h>
#include <orca/binaryswitch.h>
#include <orca/camera.h>
#include <orca/cpu.h>
#include <orca/gps.h>
#include <orca/ogmap.h>
#include <orca/home.h>
#include <orca/imu.h>
#include <orca/laserscanner2d.h>
#include <orca/position2d.h>
#include <orca/position3d.h>
#include <orca/localise2d.h>
#include <orca/platform2d.h>
#include <orca/power.h>
#include <orca/status.h>
#include <orca/tracer.h>
#include <orca/polarfeature2d.h>
#include <orca/featuremap2d.h>
#include <orca/pathfollower2d.h>
#include <orca/pathplanner2d.h>
#include <orca/tracer.h>

namespace orcaice
{

/*!
 *  @name String Utilities
 */
//@{

//! Parses the string into a sequence of strings for a given separator.
Ice::StringSeq toStringSeq( const std::string & s, const char delim=':' );
//! Combines the sequence of strings into a single string using a given separator.
std::string toString( const Ice::StringSeq & seq, const char delim=':' );

//@}

/*!
 *  @name Stringify OCM Structures
 */
//@{

//! Converts fully qualified component name (aka Adapter ID) to a string.
//! e.i. platform/component
//! @see toComponentName
std::string toString( const orca::FQComponentName & );

//! Converts fully qualified interface name to a string.
//! e.i. interface\@platform/component
//! @see toInterfaceName
std::string toString( const orca::FQInterfaceName & );

//! Converts fully qualified topic name to a string.
//! i.e. interface/topic\@platform/component
std::string toString( const orca::FQTopicName & );

//! Converts fully executable name to a string.
//! i.e. executable@host
std::string toString( const orca::FQExecutableName & name );

//! Parses a string to extract a fully qualified component name (aka Adapter ID)
//! Expects an input string in the form of @c platform/component
//! A null structure is returned if no delimeter is present.
//! Does not handle the case of multiple delimeters.
orca::FQComponentName toComponentName( const std::string & s );

//! Parses a string to extract a fully qualified interface name
//! Expects an input string in the form of @c interface@platform/component
//! A null structure is returned if either one of the delimeters is
//! absent.
orca::FQInterfaceName toInterfaceName( const std::string & s );

//! Based on the component name, returns the default topic name for Status interface.
orca::FQTopicName toStatusTopic( const orca::FQComponentName & fqCName );

//@}

/*!
 *  @name Stringify Data Objects
 */
//@{

//! Converts to string
//! @see toCartesianPoint()
std::string toString( const orca::CartesianPoint & );
//! Parses result of the correspondinng toString().
//! Returns: 0 = parsing successful, non-zero = parsing failed.
int toCartesianPoint( const std::string &, orca::CartesianPoint & );

//! Converts to string
//! @see toCartesianPoint2d()
std::string toString( const orca::CartesianPoint2d & );
//! Parses result of the correspondinng toString().
//! Returns: 0 = parsing successful, non-zero = parsing failed.
int toCartesianPoint2d( const std::string &, orca::CartesianPoint2d & );

//! Converts to string
//! @see toFrame2d()
std::string toString( const orca::Frame2d & );
//! Parses result of the correspondinng toString().
//! Returns: 0 = parsing successful, non-zero = parsing failed.
int toFrame2d( const std::string &, orca::Frame2d & );

//! Converts to string
//! @see toFrame3d()
std::string toString( const orca::Frame3d & );
//! Parses result of the correspondinng toString().
//! Returns: 0 = parsing successful, non-zero = parsing failed.
int toFrame3d( const std::string &, orca::Frame3d & );

//! Converts to string
//! @see toSize2d()
std::string toString( const orca::Size2d & );
//! Parses result of the correspondinng toString().
//! Returns: 0 = parsing successful, non-zero = parsing failed.
int toSize2d( const std::string &, orca::Size2d & );

//! Converts to string
//! @see toSize3d()
std::string toString( const orca::Size3d & );
//! Parses result of the correspondinng toString().
//! Returns: 0 = parsing successful, non-zero = parsing failed.
int toSize3d( const std::string &, orca::Size3d & );

//! For durations less than 24hrs returns string HH:MM:SS.sss.
//! Otherwise, DD:HH:MM:SS.sss
std::string toStringDuration( const orca::Time & );
//! Parses result of the correspondinng toString().
//! Returns: 0 = parsing successful, non-zero = parsing failed.
int toTimeDuration( const std::string &, orca::Time & );

//! Returns string in human readable format:
//! MM/DD/YY HH:MM:SS.sss.
std::string toString( const orca::Time & );
//! Returns string in format HH:MM:SS.sss.
std::string toString( const orca::TimeOfDay & );
//! Returns string in format YYYY/MM/DD.
std::string toString( const orca::Date & );

//! Converts to string
std::string toString( const orca::BinarySwitchDataPtr & );
//! Converts to string
std::string toString( const orca::CameraDataPtr & );
//! Converts to string
std::string toString( const orca::CameraDescriptionPtr & );
//! Converts to string
std::string toString( const orca::CpuInfoPtr & );
//! Converts to string
std::string toString( const orca::CpuDataPtr & );
//! Converts to string
std::string toString( const orca::GpsDescriptionPtr & );
//! Converts to string
std::string toString( const orca::GpsDataPtr & );
//! Converts to string
std::string toString( const orca::GpsMapGridDataPtr & );
//! Converts to string
std::string toString( const orca::GpsTimeDataPtr & );
//! Converts to string
std::string toString( const orca::HomeDataPtr & );
//! Converts to string
std::string toString( const orca::ImuDescriptionPtr & );
//! Converts to string
std::string toString( const orca::ImuDataPtr & );
//! Converts to string. Parameter skip control verbosity. 
//! No ranges are displayed if skip=-1 (default). All ranges are displayed
//! if skip=0 (none are skipped), every second is displayed if skip=1
//! (1 skipped), etc.
std::string toString( const orca::LaserScanner2dDataPtr &, int skip=-1 );
//! Converts to string. Parameter skip control verbosity. 
//! No ranges are displayed if skip=-1 (default). All ranges are displayed
//! if skip=0 (none are skipped), every second is displayed if skip=1
//! (1 skipped), etc.
std::string toString( const orca::RangeScanner2dDataPtr &, int skip=-1  );
//! Converts to string
std::string toString( const orca::RangeScanner2dDescriptionPtr & );
//! Converts to string
std::string toString( const orca::PolarFeature2dDataPtr & );
//! Converts to string
std::string toString( const orca::FeatureMap2dDataPtr & );
//! Converts to string
std::string toString( const orca::Position2dDataPtr & );
//! Converts to string
std::string toString( const orca::Position3dDataPtr & );
//! Converts to string
std::string toString( const orca::Position3dDescriptionPtr & );
//! Converts to string
std::string toString( const orca::Localise2dDataPtr & );
//! Converts to string
std::string toString( const orca::PowerDataPtr & );
//! Converts to string
std::string toString( const orca::StatusDataPtr & );
//! Converts to string
std::string toString( const orca::Velocity2dCommandPtr & );
//! Converts to string
std::string toString( const orca::TracerData & );
//! Converts to string
std::string toString( const orca::OgMapDataPtr & );
//! Converts to string
std::string toString( const orca::PathFollower2dDataPtr & );
//! Converts to string
std::string toString( const orca::Waypoint2d & );
//std::string toString( const orca::PathPlanner2dDataPtr & );
//std::string toString( const orca::PathPlanner2dTaskPtr & );

//
// More verbose versions of toString.
// Note: not all objects need support this function,
//       just those with lots of detail that's usually not necessary.
//

//! Displays the contents of the map on to the console
//! (large maps can be displyed with a small font size)
std::string toVerboseString( const orca::OgMapDataPtr & );
//! Prints out each waypoint
std::string toVerboseString( const orca::PathFollower2dDataPtr & );
//! Prints out the path and result code
std::string toVerboseString( const orca::PathPlanner2dDataPtr & );
//! Prints out the coarse path given as a task
std::string toVerboseString( const orca::PathPlanner2dTaskPtr & );
//! Prints out imu raw data
std::string toVerboseString( const orca::ImuDataPtr & );
//! Prints out position3d data
std::string toVerboseString( const orca::Position3dDataPtr & );

// Prints out all the ranges
// std::string toVerboseString( const orca::RangeScanner2dDataPtr & obj ) { return toString(obj,0); };
// Prints out all the laser ranges and intensities
// std::string toVerboseString( const orca::LaserScanner2dDataPtr & obj ) { return toString(obj,0); };

//}


} // namespace

#endif
