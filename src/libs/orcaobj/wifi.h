/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAOBJ_WIFI_H
#define ORCAOBJ_WIFI_H

#include <orca/wifi.h>

namespace orcaobj
{

// alexm: sparing it for now because it has a non-zero entry.
//! Sets all fields to zero
void setInit( orca::WifiInterface &w );

//! Converts to string
std::string toString( const orca::WifiData& );

} // namespace

#endif
