/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAOBJ_STRINGIFY_UTILITIES_H
#define ORCAOBJ_STRINGIFY_UTILITIES_H

#include <string>
#include <orca/orca.h>
#include <orca/binaryswitch.h>
#include <orca/camera.h>
#include <orca/cpu.h>
#include <orca/drivebicycle.h>
#include <orca/estop.h>
#include <orca/gps.h>
#include <orca/ogmap.h>
#include <orca/home.h>
#include <orca/image.h>
#include <orca/imu.h>
#include <orca/odometry2d.h>
#include <orca/odometry3d.h>
#include <orca/laserscanner2d.h>
#include <orca/localise2d.h>
#include <orca/localise3d.h>
#include <orca/pixmap.h>
#include <orca/power.h>
#include <orca/status.h>
#include <orca/tracer.h>
#include <orca/polarfeature2d.h>
#include <orca/featuremap2d.h>
#include <orca/pathfollower2d.h>
#include <orca/pathplanner2d.h>
#include <orca/tracer.h>
#include <orca/velocitycontrol2d.h>
#include <orca/wifi.h>
#include <orca/properties.h>

namespace orcaobj
{

/*!
 *  @name Stringify OCM Structures
 */
//@{

//! Converts fully qualified component name (aka Adapter ID) to a string.
//! e.i. platform/component
//! @see toComponentName
std::string toString( const orca::FQComponentName& );

//! Converts fully qualified interface name to a string.
//! e.i. interface\@platform/component
//! @see toInterfaceName
std::string toString( const orca::FQInterfaceName& );

//! Converts fully qualified topic name to a string.
//! i.e. interface/topic\@platform/component
std::string toString( const orca::FQTopicName& );

//! Converts fully executable name to a string.
//! i.e. executable@host
std::string toString( const orca::FQExecutableName & name );

//! Parses a string to extract a fully qualified component name (aka Adapter ID)
//! Expects an input string in the form of @c platform/component
//! A null structure is returned if no delimeter is present.
//! Does not handle the case of multiple delimeters.
orca::FQComponentName toComponentName( const std::string& s );

//! Parses a string to extract a fully qualified interface name
//! Expects an input string in the form of @c interface@platform/component
//! A null structure is returned if either one of the delimeters is
//! absent.
orca::FQInterfaceName toInterfaceName( const std::string& s );

//! Based on the component name, returns the default identity of Home interface.
std::string toHomeIdentity( const orca::FQComponentName & fqCName );

//! Based on the component name, returns the default topic name for Status interface.
orca::FQTopicName toStatusTopic( const orca::FQComponentName & fqCName );

//! Based on the component name, returns the default topic name for Tracer interface.
orca::FQTopicName toTracerTopic( const orca::FQComponentName & fqCName );

//@}

/*!
 *  @name Stringify Data Objects
 */
//@{

//! Parses result of the ifacestring::toString().
//! Returns: 0 = parsing successful, non-zero = parsing failed.
int toImageFormat( const std::string &, orca::ImageFormat& );

//! Converts to string
//! @see toCartesianPoint()
std::string toString( const orca::CartesianPoint& );
//! Parses result of the correspondinng toString().
//! Returns: 0 = parsing successful, non-zero = parsing failed.
int toCartesianPoint( const std::string &, orca::CartesianPoint& );

//! Converts to string
//! @see toCartesianPoint2d()
std::string toString( const orca::CartesianPoint2d& );
//! Parses result of the correspondinng toString().
//! Returns: 0 = parsing successful, non-zero = parsing failed.
int toCartesianPoint2d( const std::string &, orca::CartesianPoint2d& );

//! Converts to string
//! @see toFrame2d()
std::string toString( const orca::Frame2d& );
//! Parses result of the correspondinng toString().
//! Returns: 0 = parsing successful, non-zero = parsing failed.
int toFrame2d( const std::string &, orca::Frame2d& );
//! Converts to string
std::string toString( const orca::Covariance2d& );

//! Converts to string
//! @see toFrame3d()
std::string toString( const orca::Frame3d& );
//! Parses result of the correspondinng toString().
//! Returns: 0 = parsing successful, non-zero = parsing failed.
int toFrame3d( const std::string &, orca::Frame3d& );

//! Converts to string
//! @see toSize2d()
std::string toString( const orca::Size2d& );
//! Parses result of the correspondinng toString().
//! Returns: 0 = parsing successful, non-zero = parsing failed.
int toSize2d( const std::string &, orca::Size2d& );

//! Converts to string
//! @see toSize3d()
std::string toString( const orca::Size3d& );
//! Parses result of the correspondinng toString().
//! Returns: 0 = parsing successful, non-zero = parsing failed.
int toSize3d( const std::string &, orca::Size3d& );

//! Parses string of ints separated by spaces.
//! Returns: 0 = parsing successful, non-zero = parsing failed.
int toIntVector( const std::string &, std::vector<int>& );

//! Parses string of doubles separated by spaces.
//! Returns: 0 = parsing successful, non-zero = parsing failed.
int toDoubleVector( const std::string &, std::vector<double>& );

//! For durations less than 24hrs returns string HH:MM:SS.sss.
//! Otherwise, DD:HH:MM:SS.sss
//! Negative times are supported but only if seconds and microseconds
//! are both negative.
std::string toStringDuration( const orca::Time& );
//! Parses result of the correspondinng toString().
//! Returns: 0 = parsing successful, non-zero = parsing failed.
//! Negative durations are supported, but only if the very first 
//! character of the string is '-'.
int toTimeDuration( const std::string &, orca::Time& );

//! Returns string in human readable format:
//! MM/DD/YY HH:MM:SS.sss.
std::string toString( const orca::Time& );
//! Returns string in format HH:MM:SS.sss.
std::string toString( const orca::TimeOfDay& );
//! Returns string in format YYYY/MM/DD.
std::string toString( const orca::Date& );

//! Converts to string
std::string toString( const orca::BinarySwitchData& );
//! Converts to string
std::string toString( const orca::CameraDataSequence& );
//! Converts to string
std::string toString( const orca::CameraDescriptionSequence& );
//! Converts to string
std::string toString( const orca::CpuInfo& );
//! Converts to string
std::string toString( const orca::CpuData& );
//! Converts to string
std::string toString( const orca::DriveBicycleCommand& );
//! Converts to string
std::string toString( const orca::DriveBicycleData& );
//! Converts to string
std::string toString( const orca::EStopData& );
//! Converts to string
std::string toString( const orca::GpsDescription& );
//! Converts to string
std::string toString( const orca::GpsData&  );
//! Converts to string
std::string toString( const orca::HomeData& );
//! Converts to string
std::string toString( const orca::ImuDescription& );
//! Converts to string
std::string toString( const orca::ImageDataPtr& );
//! Converts to string
std::string toString( const orca::ImageDescriptionPtr& );
//! Converts to string
std::string toString( const orca::ImuData& );
//! Converts to string
std::string toString( const orca::Odometry2dData& );
//! Converts to string
std::string toString( const orca::Odometry3dData& );
//! Converts to string
std::string toString( const orca::VehicleDescription& );
//! Converts to string
std::string toString( const orca::VehicleControlDescriptionPtr& );
//! Converts to string
std::string toString( const orca::VehicleGeometryDescriptionPtr& );
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
std::string toString( const orca::RangeScanner2dDescription& );
//! Converts to string
std::string toString( const orca::SinglePolarFeature2dPtr& obj );
//! Converts to string
std::string toString( const orca::PolarFeature2dData& );
//! Converts to string
std::string toString( const orca::FeatureMap2dData& );
//! Converts to string
std::string toString( const orca::Feature2d& );
//! Converts to string
std::string toString( const orca::Twist2d& );
//! Converts to string
std::string toString( const orca::Localise2dData& );
//! Converts to string
std::string toString( const orca::Localise3dData& );
//! Converts to string
std::string toString( const orca::BatteryData& );
//! Converts to string
std::string toString( const orca::BatteriesData& );
//! Converts to string
std::string toString( const orca::PowerData& );
//! Converts to string
std::string toString( const orca::PixMapData& );
//! Converts to string
std::string toString( const orca::StatusData& );
//! Converts to string
std::string toString( const orca::VelocityControl2dData& );
//! Converts to string
std::string toString( const orca::TracerData& );
//! Converts to string
std::string toString( const orca::OgMapData& );
//! Converts to string
std::string toString( const orca::PathFollower2dData& );
//! Converts to string
std::string toString( const orca::Waypoint2d& );
//! Converts to string
std::string toString( const orca::WifiData& );
//! Converts to string
std::string toString( const orca::PropertiesData& );

//
// More verbose versions of toString.
// Note: not all objects need support this function,
//       just those with lots of detail that's usually not necessary.
//

//! Displays the contents of the map on to the console
//! (large maps can be displyed with a small font size)
std::string toVerboseString( const orca::OgMapData& );
//! Prints out each waypoint
std::string toVerboseString( const orca::PathFollower2dData& );
//! Prints out the path and result code
std::string toVerboseString( const orca::PathPlanner2dData& );
//! Prints out the coarse path given as a task
std::string toVerboseString( const orca::PathPlanner2dTask& );
//! Prints out imu raw data
std::string toVerboseString( const orca::ImuData& );

// Prints out all the ranges
// std::string toVerboseString( const orca::RangeScanner2dDataPtr & obj ) { return toString(obj,0); };
// Prints out all the laser ranges and intensities
// std::string toVerboseString( const orca::LaserScanner2dDataPtr & obj ) { return toString(obj,0); };

//}


} // namespace

#endif
