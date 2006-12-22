/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TRACERMON_EVENTS_H
#define ORCA2_TRACERMON_EVENTS_H

#include <orcaice/eventqueue.h>

namespace tracermon
{

enum EventType  
{
    // user
    VerbosityLevelsChanged=0,
    // network
    NewTraceMessage
};

class NewTraceMessageEvent : public orcaice::Event
{
public:
    NewTraceMessageEvent( const orca::TracerData & data )
        : Event( EventType(NewTraceMessage) ),
          data_(data)  {};

    orca::TracerData data_;
};

} // namespace

#endif
