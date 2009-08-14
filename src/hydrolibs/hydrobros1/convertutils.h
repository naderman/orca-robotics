/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDROBROS1_CONVERT_UTILITIES_H
#define HYDROBROS1_CONVERT_UTILITIES_H

#include <hydrobros1/types.h>
#include <hydroportability/sharedlib.h>

namespace hydrobros1
{

//! Converts from 3d to 2d structure.
SOEXPORT void convert( const hydrobros1::Size3d& obj3d, hydrobros1::Size2d& obj2d );
//! Converts from 3d to 2d structure.
SOEXPORT void convert( const hydrobros1::CartesianPoint3d& obj3d, hydrobros1::CartesianPoint2d& obj2d );

} // namespace

#endif
