/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCALOG_STRINGIFY_UTILITIES_H
#define ORCALOG_STRINGIFY_UTILITIES_H

#include <string>
#include <orca/orca.h>
#include <orca/binaryswitch.h>
#include <orca/camera.h>
#include <orca/cpu.h>
#include <orca/drivebicycle.h>
#include <orca/gps.h>
#include <orca/ogmap.h>
#include <orca/home.h>
#include <orca/imu.h>
#include <orca/laserscanner2d.h>
#include <orca/odometry2d.h>
#include <orca/odometry3d.h>
#include <orca/localise2d.h>
#include <orca/localise3d.h>
#include <orca/power.h>
#include <orca/status.h>
//#include <orca/polarfeature2d.h>
#include <orca/featuremap2d.h>
#include <orca/pathfollower2d.h>
#include <orca/pathplanner2d.h>
#include <orca/wifi.h>

namespace orcalog
{
/*!
 *  @name Stringify Orca structures for logging.
 */
//@{

//! Prints out waypoint2d to text which is easy to parse
std::string toLogString( const orca::Waypoint2d& );
//! Parses from ASCII log
void fromLogString( std::stringstream&, orca::Waypoint2d& );
//! Prints out Path2d to text which is easy to parse
std::string toLogString( const orca::Path2d& );
//! Prints out cpu data to text which is easy to parse
std::string toLogString( const orca::CpuData& );

//! Prints out gps data to text which is easy to parse
std::string toLogString( const orca::GpsData& );
//! Parses from ASCII log. Throws orcalog::ParseException on error.
void fromLogString( std::stringstream& stream, orca::GpsData& );

//! Prints out gps mapgrid data to text which is easy to parse
std::string toLogString( const orca::GpsMapGridData& );
//! Parses from ASCII log. Throws orcalog::ParseException on error.
void fromLogString( std::stringstream& stream, orca::GpsMapGridData& );

//! Prints out gps time data to text which is easy to parse
std::string toLogString( const orca::GpsTimeData& );
//! Parses from ASCII log. Throws orcalog::ParseException on error.
void fromLogString( std::stringstream& stream, orca::GpsTimeData& );

std::string toLogString( const orca::LaserScanner2dDataPtr& );
std::string toLogString( const orca::Localise2dData& );
std::string toLogString( const orca::Localise3dData& );
//! Prints out resulting fine path to text in a parsable format
std::string toLogString( const orca::PathPlanner2dData& );
//! Prints out task path (coarse) to text in a parsable format
std::string toLogString( const orca::PathPlanner2dTask& );
// //! Prints out polarfeature2d data to text which is easy to parse
// std::string toLogString( const orca::PolarFeature2dData& )
std::string toLogString( const orca::Odometry2dData& );
std::string toLogString( const orca::Odometry3dData& );
//! Prints out imu data to text which is easy to parse
std::string toLogString( const orca::ImuData& obj );
//! Prints out wifi data to text which is easy to parse
std::string toLogString( const orca::WifiData& obj );
std::string toLogString( const orca::DriveBicycleData& );

//! Prints out Power data to text which is easy to parse
std::string toLogString( const orca::PowerData& );
//! Parses a line of ASCII log. Throws orcalog::ParseException on error.
void fromLogString( std::stringstream& stream, orca::PowerData& );

//! Returns string in simple format "sec usec" 
std::string toLogString( const orca::Time& );
//! Parses a line of ASCII log. Throws orcalog::ParseException on error.
void fromLogString( std::stringstream& stream, orca::Time& );

//! Prints out gpsdescription to text which is easy to parse
std::string toLogString( const orca::GpsDescription& );
//! Parses from ASCII log. Throws orcalog::ParseException on error.
void fromLogString( std::stringstream& stream, orca::GpsDescription& );

//! Converts vehicle geometry to text which is easy to parse
std::string toLogString( const orca::VehicleGeometryDescriptionPtr& );
//}


} // namespace

#endif
