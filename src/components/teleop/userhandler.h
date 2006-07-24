/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TELEOP_USER_HANDLER_H
#define ORCA2_TELEOP_USER_HANDLER_H

#include <orcaice/thread.h>
#include <orcaice/context.h>
#include <orcaice/ptrbuffer.h>

#include <orca/platform2d.h>

#include "inputdriver.h"

namespace teleop
{

class DisplayHandler;

class UserHandler : public orcaice::Thread
{
public:

    UserHandler( orcaice::PtrBuffer<orca::Velocity2dCommandPtr> *commands,
                    const orcaice::Context & context );
    virtual ~UserHandler();

    virtual void run();

    DisplayHandler* displayHandler() { return displayHandler_; };
    
private:

    // network/driver interface
    orcaice::PtrBuffer<orca::Velocity2dCommandPtr> *commandPipe_;

    // generic interface to input hardware
    InputDriver* driver_;

    InputDriver::Config config_;

    DisplayHandler* displayHandler_;
    
    void init();

    // component current context
    orcaice::Context context_;
};

} // namespace

#endif
