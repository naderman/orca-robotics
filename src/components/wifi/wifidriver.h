/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA_WIFI_DRIVER_H
#define ORCA_WIFI_DRIVER_H

#include <vector>
#include <orca/wifi.h>

namespace wifi
{

//
// Base-class for wifi drivers
//
class WifiDriver 
{
public:
    virtual ~WifiDriver() {};
    
    virtual void init() = 0;
    virtual void read( orca::WifiData &data ) = 0;
};

} // namespace

#endif
