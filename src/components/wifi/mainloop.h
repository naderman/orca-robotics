/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA2_WIFI_MAINLOOP_H
#define ORCA2_WIFI_MAINLOOP_H

#include <orcaice/thread.h>
#include <orcaifaceimpl/wifiiface.h>

namespace wifi
{
    
class WifiDriver;

class MainLoop : public orcaice::Thread
{

public: 

    MainLoop( const orcaice::Context & context );
    ~MainLoop();

    virtual void run();

private:
    orcaice::Context context_;
    orcaifaceimpl::WifiIfacePtr wifiInterface_;
    WifiDriver *driver_;
    
    void initNetwork();
    void initDriver();
    
};

}

#endif
