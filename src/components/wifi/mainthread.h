/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA2_WIFI_MAINTHREAD_H
#define ORCA2_WIFI_MAINTHREAD_H

#include <hydroutil/subsystemthread.h>
#include <orcaifaceimpl/wifiImpl.h>

namespace wifi
{
    
class WifiDriver;

class MainThread : public hydroutil::SubsystemThread
{

public: 

    MainThread( const orcaice::Context & context );

    virtual void walk();

private:
    orcaice::Context context_;
    orcaifaceimpl::WifiImplPtr wifiInterface_;
    WifiDriver *driver_;
    
    int snrWarningThreshhold_;
    
    void initNetworkInterface();
    void initHardwareDriver();
    void checkWifiSignal( orca::WifiData &data );
    
};

}

#endif
