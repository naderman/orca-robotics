/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TELEOP_INPUT_HANDLER_H
#define ORCA2_TELEOP_INPUT_HANDLER_H

#include <orcaice/thread.h>
#include <orcaice/context.h>

namespace teleop
{

class Network;
class InputDriver;

class InputHandler : public orcaice::Thread
{
public:

    InputHandler( Network* network, const orcaice::Context& context );
    virtual ~InputHandler();

    // from Thread
    virtual void run();

private:

    Network* network_;

    InputDriver* driver_;

    // component current context
    orcaice::Context context_;

};

} // namespace

#endif
