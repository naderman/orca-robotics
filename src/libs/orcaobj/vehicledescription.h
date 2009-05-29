/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAOBJ_VEHICLE_DESCRIPTION_H
#define ORCAOBJ_VEHICLE_DESCRIPTION_H

#include <orca/vehicledescription.h>

namespace orcaobj
{

//! Converts to string
std::string toString( const orca::VehicleDescription& );
//! Converts to string
std::string toString( const orca::VehicleControlDescriptionPtr& );
//! Converts to string
std::string toString( const orca::VehicleGeometryDescriptionPtr& );

} // namespace

#endif
