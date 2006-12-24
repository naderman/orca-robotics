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
#include <orca/tracer.h>

namespace tracermon
{

enum EventType  
{
    // user
    VerbosityLevelsChanged=0,
    // network
    NewTraceMessage
};

class VerbosityLevelsChangedEvent : public orcaice::Event
{
public:
    VerbosityLevelsChangedEvent( int error, int warn, int info, int debug ) :
        Event( VerbosityLevelsChanged ),
        error_(error),
        warn_(warn),
        info_(info),
        debug_(debug) {};

    int error_;
    int warn_;
    int info_;
    int debug_;
};
typedef IceUtil::Handle<VerbosityLevelsChangedEvent> VerbosityLevelsChangedEventPtr;


class NewTraceMessageEvent : public orcaice::Event
{
public:
    NewTraceMessageEvent( const orca::TracerData & data ) :
        Event( NewTraceMessage ),
        data_(data)  {};

    orca::TracerData data_;
};
typedef IceUtil::Handle<NewTraceMessageEvent> NewTraceMessageEventPtr;


} // namespace

#endif
