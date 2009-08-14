/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDROBROS1_STRINGIFY_UTILITIES_H
#define HYDROBROS1_STRINGIFY_UTILITIES_H

#include <string>
#include <hydrobros1/types.h>
#include <hydroportability/sharedlib.h>

namespace hydrobros1
{

//! Converts to string
//! @see toCartesianPoint3d()
SOEXPORT std::string toString( const hydrobros1::CartesianPoint3d& );
//! Parses result of the correspondinng toString().
//! Returns: 0 = parsing successful, non-zero = parsing failed.
SOEXPORT int toCartesianPoint( const std::string &, hydrobros1::CartesianPoint3d& );

//! Converts to string
//! @see toCartesianPoint2d()
SOEXPORT std::string toString( const hydrobros1::CartesianPoint2d& );
//! Parses result of the correspondinng toString().
//! Returns: 0 = parsing successful, non-zero = parsing failed.
SOEXPORT int toCartesianPoint2d( const std::string &, hydrobros1::CartesianPoint2d& );

//! Converts to string
//! @see toFrame2d()
SOEXPORT std::string toString( const hydrobros1::Frame2d& );
//! Parses result of the correspondinng toString().
//! Returns: 0 = parsing successful, non-zero = parsing failed.
SOEXPORT int toFrame2d( const std::string &, hydrobros1::Frame2d& );

//! Converts to string
//! @see toFrame3d()
SOEXPORT std::string toString( const hydrobros1::Frame3d& );
//! Parses result of the correspondinng toString().
//! Returns: 0 = parsing successful, non-zero = parsing failed.
SOEXPORT int toFrame3d( const std::string &, hydrobros1::Frame3d& );

//! Converts to string
//! @see toSize2d()
SOEXPORT std::string toString( const hydrobros1::Size2d& );
//! Parses result of the correspondinng toString().
//! Returns: 0 = parsing successful, non-zero = parsing failed.
SOEXPORT int toSize2d( const std::string &, hydrobros1::Size2d& );

//! Converts to string
//! @see toSize3d()
SOEXPORT std::string toString( const hydrobros1::Size3d& );
//! Parses result of the correspondinng toString().
//! Returns: 0 = parsing successful, non-zero = parsing failed.
SOEXPORT int toSize3d( const std::string &, hydrobros1::Size3d& );

//! Converts to string
SOEXPORT std::string toString( const hydrobros1::Twist2d& );

} // namespace

#endif
