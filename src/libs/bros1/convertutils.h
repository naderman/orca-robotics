/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef BROS1_CONVERT_UTILITIES_H
#define BROS1_CONVERT_UTILITIES_H

#include <bros1/types.h>

namespace bros1
{

//! Converts from 3d to 2d structure.
void convert( const bros1::Size3d& obj3d, bros1::Size2d& obj2d );
//! Converts from 3d to 2d structure.
void convert( const bros1::CartesianPoint3d& obj3d, bros1::CartesianPoint2d& obj2d );

} // namespace

#endif
