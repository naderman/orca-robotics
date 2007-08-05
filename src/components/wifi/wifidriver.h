/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA2_WIFI_DRIVER_H
#define ORCA2_WIFI_DRIVER_H

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
