/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCABROS1_CONVERT_UTILITIES_H
#define ORCABROS1_CONVERT_UTILITIES_H

// BROS-I data structures defined in Slice
#include <orca/bros1.h>
// BROS-I data structures defined directly in C++
#include <hydrobros1/types.h>

namespace orcabros1
{
/*!
 *  @name C++ to Slice Conversion Functions
 */
//@{

//! Converts from C++ structure to Slice-defined structure.
void convert( const hydrobros1::Size2d& cobj, orca::Size2d& iceobj );
//! Converts from C++ structure to Slice-defined structure.
void convert( const hydrobros1::CartesianPoint2d& cobj, orca::CartesianPoint2d& iceobj );
//! Converts from C++ structure to Slice-defined structure.
void convert( const hydrobros1::CartesianPoint3d& cobj, orca::CartesianPoint& iceobj );
//! Converts from C++ structure to Slice-defined structure and reduce dimensions.
void convert( const hydrobros1::CartesianPoint3d& cobj, orca::CartesianPoint2d& iceobj );
//! Converts from C++ structure to Slice-defined structure.
void convert( const hydrobros1::Frame3d& cobj, orca::Frame3d& iceobj );

//@}

/*!
 *  @name Slice to C++ Conversion Functions
 */
//@{
//! Converts from Slice-defined structure to C++ structure.
void convert( const orca::Size2d& iceobj, hydrobros1::Size2d& cobj );
//! Converts from Slice-defined structure to C++ structure.
void convert( const orca::CartesianPoint2d& iceobj, hydrobros1::CartesianPoint2d& cobj );
//! Converts from Slice-defined structure to C++ structure.
void convert( const orca::CartesianPoint& iceobj, hydrobros1::CartesianPoint3d& cobj );
//! Converts from Slice-defined structure to C++ structure and reduce dimensions.
void convert( const orca::CartesianPoint& iceobj, hydrobros1::CartesianPoint2d& cobj );
//! Converts from Slice-defined structure to C++ structure.
void convert( const orca::Frame3d& iceobj, hydrobros1::Frame3d& cobj );
//@}

} // namespace

#endif
