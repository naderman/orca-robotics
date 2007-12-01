/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA2_BRICK_MAIN_THREAD_H
#define ORCA2_BRICK_MAIN_THREAD_H

#include <hydroutil/safethread.h>
#include <orcaice/context.h>

namespace brick
{

class MainThread: public hydroutil::SafeThread
{    	
public:
    MainThread( const orcaice::Context& context );
    virtual ~MainThread();

    // from SafeThread
    virtual void walk();

private:
    orcaice::Context context_;

};

} // namespace

#endif
