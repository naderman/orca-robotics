/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef  TERMINAL_DISPLAY_THREAD_H
#define  TERMINAL_DISPLAY_THREAD_H

#include <hydroutil/safethread.h>
#include <orcaice/context.h>
#include <hydroutil/eventqueue.h>

#include "display.h"

namespace teleop
{

class TermDisplayDriver;

class TermDisplayThread : public hydroutil::SafeThread, public Display
{
public:

    TermDisplayThread( const orcaice::Context& context );
    virtual ~TermDisplayThread();

    // from Thread
    virtual void walk();

    // from Display
    virtual void sentNewVelocityCommand( 
                double vx, double vy, double w,
                bool vxLimit=false, bool vyLimit=false, bool wLimit=false );

    virtual void sentNewBicycleCommand( 
                double speed, double steerAngle,
                bool speedLimit=false, bool steerAngleLimit=false );

    virtual void sentRepeatCommand();

    virtual void failedToSendCommand();
    
private:

    hydroutil::EventQueuePtr events_;

    // generic interface to input hardware
    TermDisplayDriver* driver_;

    // component current context
    orcaice::Context context_;
};

} // namespace

#endif
