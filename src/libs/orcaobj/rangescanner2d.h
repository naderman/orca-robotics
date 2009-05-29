/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAOBJ_RANGESCANNER2D_H
#define ORCAOBJ_RANGESCANNER2D_H

#include <orca/rangescanner2d.h>

namespace orcaobj
{

//! Calculates the angle increment for a range scan.
//! This is in a special function because it's easy to forget the '+1'
inline double calcAngleIncrement( double fieldOfView, int numReturns )
{ return fieldOfView / double(numReturns-1); }

//! Converts to string
std::string toString( const orca::RangeScanner2dDescription& );

} // namespace

#endif
