/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA2_WIFI_MAINLOOP_H
#define ORCA2_WIFI_MAINLOOP_H

#include <hydroutil/thread.h>
#include <orcaifaceimpl/wifiImpl.h>

namespace wifi
{
    
class WifiDriver;

class MainLoop : public hydroutil::Thread
{

public: 

    MainLoop( const orcaice::Context & context );
    ~MainLoop();

    virtual void run();

private:
    orcaice::Context context_;
    orcaifaceimpl::WifiImplPtr wifiInterface_;
    WifiDriver *driver_;
    
    int snrWarningThreshhold_;
    
    void initNetwork();
    void initDriver();
    void checkWifiSignal( orca::WifiData &data );
    
};

}

#endif
