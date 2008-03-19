/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef MAINTHREAD_H
#define MAINTHREAD_H

#include <gbxsickacfr/gbxiceutilacfr/safethread.h>
#include <orcaice/context.h>

namespace player2orca
{

class MainThread: public gbxsickacfr::gbxiceutilacfr::SafeThread
{    	
public:
    MainThread( const orcaice::Context& context );
    ~MainThread();

    virtual void walk();

private:
    orcaice::Context context_;

};

} // namespace
#endif


