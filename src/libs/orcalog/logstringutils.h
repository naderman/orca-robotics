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
//! Prints out cpu data to text which is easy to parse
std::string toLogString( const orca::CpuData& );
//! Prints out gps data to text which is easy to parse
std::string toLogString( const orca::GpsData& );
//! Prints out gps mapgrid data to text which is easy to parse
std::string toLogString( const orca::GpsMapGridData& );
//! Prints out laser data to text which is easy to parse
std::string toLogString( const orca::LaserScanner2dDataPtr& );
//! Prints out localise2d data to text which is easy to parse
std::string toLogString( const orca::Localise2dData& );
//! Prints out localise3d data to text which is easy to parse
std::string toLogString( const orca::Localise3dData& );
//! Prints out resulting fine path to text in a parsable format
std::string toLogString( const orca::PathPlanner2dData& );
//! Prints out task path (coarse) to text in a parsable format
std::string toLogString( const orca::PathPlanner2dTask& );
// //! Prints out polarfeature2d data to text which is easy to parse
// std::string toLogString( const orca::PolarFeature2dData& );
//! Prints out Odometry2d data to text which is easy to parse
std::string toLogString( const orca::Odometry2dData& );
//! Prints out Odometry3d data to text which is easy to parse
std::string toLogString( const orca::Odometry3dData& );
//! Prints out imu data to text which is easy to parse
std::string toLogString( const orca::ImuData& obj );
//! Prints out wifi data to text which is easy to parse
std::string toLogString( const orca::WifiData& obj );

//! Prints out Power data to text which is easy to parse
std::string toLogString( const orca::PowerData& );
//! Parses a line of ASCII log. Throws orcalog::ParseException on error.
void fromLogString( std::stringstream& stream, orca::PowerData& );

//! Returns string in simple format "sec usec" 
std::string toLogString( const orca::Time& );
//! Parses a line of ASCII log. Throws orcalog::ParseException on error.
void fromLogString( std::stringstream& stream, orca::Time& );
//}


} // namespace

#endif
