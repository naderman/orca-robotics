/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef MAINLOOP_H
#define MAINLOOP_H

#include <hydroutil/thread.h>
#include <orcaice/context.h>
#include <hydroutil/proxy.h>

#include <orca/laserscanner2d.h>
#include <IceUtil/Time.h>

//
// @author Alex Brooks
//
class MainLoop : public hydroutil::Thread
{

public: 

    MainLoop( const orcaice::Context & context );
    ~MainLoop();

    virtual void run();

private: 

    bool possiblyReconfigure( bool wasEnabled );
    int  msSinceReconfigure();

    orcaice::Context                               context_;

    IceUtil::Time                               lastConfigChangeTime_;
    bool                                        laserOperating_;
    double                                      msTillNextReconfigure_;

    static const int MAX_MS_BETWEEN_RECONFIGURATIONS  = 100000;
};

#endif
