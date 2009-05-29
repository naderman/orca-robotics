/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA_HARDWARE_DRIVER_H
#define ORCA_HARDWARE_DRIVER_H

#include <orcaice/context.h>
#include <hydrowifi/wifiutil.h>

#include "wifidriver.h"

namespace wifi
{

class HardwareDriver : public WifiDriver 
{
public:
    
    HardwareDriver( orcaice::Context &context ); 
    ~HardwareDriver() {};
    
    virtual void init();
    virtual void read( orca::WifiData &data );
    
private:
    orcaice::Context context_;
    bool haveIoctl_;
};

} // namespace

#endif
