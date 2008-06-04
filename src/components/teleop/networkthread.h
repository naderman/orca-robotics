/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef  NETWORK_THREAD_H
#define  NETWORK_THREAD_H

#include <gbxsickacfr/gbxiceutilacfr/safethread.h>
#include <orcaice/context.h>
#include <hydroiceutil/eventqueue.h>

#include "network.h"

namespace teleop
{

class Display;
class NetworkDriver;

class NetworkThread : public gbxiceutilacfr::SafeThread, public Network
{
public:

    NetworkThread( Display* display, const orcaice::Context& context );
    virtual ~NetworkThread();

    // from SafeThread
    virtual void walk();

    // from Network
    virtual void newMixedCommand( const hydrointerfaces::HumanInput2d::Command& command );

    virtual void newIncrementCommand( int longitudinal, int transverse, int angle );
    virtual void newRelativeCommand( double longitudinal, double transverse, double angle );


private:

    hydroiceutil::EventQueuePtr events_;

    Display* display_;

    NetworkDriver* driver_;

    // component current context
    orcaice::Context context_;

};

} // namespace

#endif
