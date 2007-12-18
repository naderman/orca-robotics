/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA_FAKE_DRIVER_H
#define ORCA_FAKE_DRIVER_H

#include "wifidriver.h"

namespace wifi
{

class FakeDriver : public WifiDriver 
{
public:
    FakeDriver() {}; 
    ~FakeDriver() {};
    
    virtual void init() {};
    virtual void read( orca::WifiData &data );
    
};

} // namespace

#endif
