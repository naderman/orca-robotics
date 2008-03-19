/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef  DISPLAY_THREAD_H
#define  DISPLAY_THREAD_H

#include <gbxsickacfr/gbxiceutilacfr/safethread.h>
#include <orcaice/context.h>

#include "inputdriver.h"

namespace teleop
{

class DisplayThread : public gbxsickacfr::gbxiceutilacfr::SafeThread, public Display
{
public:

    DisplayThread( const orcaice::Context& context );
    virtual ~DisplayThread();

    virtual void walk();

//     DisplayThread* displayMainThread() { return displayMainThread_; };
    
private:

    // generic interface to input hardware
    InputDriver* driver_;

    InputDriver::Config config_;

//     DisplayThread* displayMainThread_;
    
    void init();

    // component current context
    orcaice::Context context_;
};

} // namespace

#endif
