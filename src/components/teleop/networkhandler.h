/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TELEOP_NETWORK_HANDLER_H
#define ORCA2_TELEOP_NETWORK_HANDLER_H

#include <orcaiceutil/safethread.h>
#include <orcaice/context.h>
#include <orcaiceutil/eventqueue.h>

#include "network.h"

namespace teleop
{

class Display;
class NetworkDriver;

class NetworkHandler : public orcaiceutil::SafeThread, public Network
{
public:

    NetworkHandler( Display* display, const orcaice::Context& context );
    virtual ~NetworkHandler();

    // from SafeThread
    virtual void walk();

    // from Network
    virtual void newCommandIncrement( int longitudinal, int transverse, int angle );

    virtual void newRelativeCommand( double longitudinal, double transverse, double angle );


private:

    orcaiceutil::EventQueuePtr events_;

    Display* display_;

    NetworkDriver* driver_;

    // component current context
    orcaice::Context context_;

};

} // namespace

#endif
