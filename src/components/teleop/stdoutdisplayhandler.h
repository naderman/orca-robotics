/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TELEOP_STDOUT_DISPLAY_HANDLER_H
#define ORCA2_TELEOP_STDOUT_DISPLAY_HANDLER_H

#include "displayhandler.h"

namespace teleop
{

class StdoutDisplayHandler : public DisplayHandler
{
public:

    virtual void displayEvent( const Event e );

    virtual void displayCommand( const orca::Velocity2dCommand& command,
                                 const bool vx=false, const bool vy=false, const bool w=false );
};

} // namespace

#endif
