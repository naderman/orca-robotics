/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef BROS1_STRINGIFY_UTILITIES_H
#define BROS1_STRINGIFY_UTILITIES_H

#include <string>
#include <bros1/types.h>

namespace bros1
{

//! Converts to string
//! @see toCartesianPoint3d()
std::string toString( const bros1::CartesianPoint3d& );
//! Parses result of the correspondinng toString().
//! Returns: 0 = parsing successful, non-zero = parsing failed.
int toCartesianPoint( const std::string &, bros1::CartesianPoint3d& );

//! Converts to string
//! @see toCartesianPoint2d()
std::string toString( const bros1::CartesianPoint2d& );
//! Parses result of the correspondinng toString().
//! Returns: 0 = parsing successful, non-zero = parsing failed.
int toCartesianPoint2d( const std::string &, bros1::CartesianPoint2d& );

//! Converts to string
//! @see toFrame2d()
std::string toString( const bros1::Frame2d& );
//! Parses result of the correspondinng toString().
//! Returns: 0 = parsing successful, non-zero = parsing failed.
int toFrame2d( const std::string &, bros1::Frame2d& );

//! Converts to string
//! @see toFrame3d()
std::string toString( const bros1::Frame3d& );
//! Parses result of the correspondinng toString().
//! Returns: 0 = parsing successful, non-zero = parsing failed.
int toFrame3d( const std::string &, bros1::Frame3d& );

//! Converts to string
//! @see toSize2d()
std::string toString( const bros1::Size2d& );
//! Parses result of the correspondinng toString().
//! Returns: 0 = parsing successful, non-zero = parsing failed.
int toSize2d( const std::string &, bros1::Size2d& );

//! Converts to string
//! @see toSize3d()
std::string toString( const bros1::Size3d& );
//! Parses result of the correspondinng toString().
//! Returns: 0 = parsing successful, non-zero = parsing failed.
int toSize3d( const std::string &, bros1::Size3d& );

//! Converts to string
std::string toString( const bros1::Twist2d& );

} // namespace

#endif
