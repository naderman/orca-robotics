/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TELEOP_TERMINAL_DISPLAY_HANDLER_H
#define ORCA2_TELEOP_TERMINAL_DISPLAY_HANDLER_H

#include <hydroutil/thread.h>
#include <orcaice/context.h>
#include <hydroutil/eventqueue.h>

#include "display.h"

namespace teleop
{

class TermDisplayDriver;

class TermDisplayHandler : public hydroutil::Thread, public Display
{
public:

    TermDisplayHandler( const orcaice::Context& context );
    virtual ~TermDisplayHandler();

    // from Thread
    virtual void run();

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
