/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TELEOP_DISPLAY_HANDLER_H
#define ORCA2_TELEOP_DISPLAY_HANDLER_H

#include <orca/platform2d.h>

namespace teleop
{

class DisplayHandler
{
public:

    enum Event
    {
        SentNewCommand,
        SentRepeatCommand,
        FailedToSendCommand
    };

    virtual ~DisplayHandler() {};

    virtual void displayEvent( const Event e )=0;
    
    virtual void displayCommand( const orca::Velocity2dCommandPtr & command,
                                 const bool vx=false, const bool vy=false, const bool w=false )=0;
};

} // namespace

#endif
