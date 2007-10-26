/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA2_HARDWARE_DRIVER_H
#define ORCA2_HARDWARE_DRIVER_H

#include <orcaice/orcaice.h>
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
