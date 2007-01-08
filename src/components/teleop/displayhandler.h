/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TELEOP_DISPLAY_HANDLER_H
#define ORCA2_TELEOP_DISPLAY_HANDLER_H

#include <orcaice/thread.h>
#include <orcaice/context.h>

#include "inputdriver.h"

namespace teleop
{

class DisplayHandler : public orcaice::Thread, public Display
{
public:

    DisplayHandler( const orcaice::Context& context );
    virtual ~DisplayHandler();

    virtual void run();

    DisplayHandler* displayHandler() { return displayHandler_; };
    
private:

    // network/driver interface
    orcaice::Buffer<orca::Velocity2dCommand> *commandPipe_;

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
