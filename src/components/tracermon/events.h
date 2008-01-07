/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef EVENTS_H
#define EVENTS_H

#include <hydroiceutil/eventqueue.h>
#include <orca/tracer.h>

namespace tracermon
{

enum EventType  
{
    // user & network
    VerbosityLevelsChanged=0,
    FocusChanged,
    NextField,
    PreviousField,
    ValueUp,
    ValueDown,
    Action,
    // network
    NewTraceMessage,
    NewLocalTrace
};
/*
enum VerbosityType
{
    ErrorType,
    WarningType,
    InfoType,
    DebugType,
    NumberOfVerbosityTypes
};
*/
class VerbosityLevelsChangedEvent : public hydroiceutil::Event
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

class FocusChangedEvent : public hydroiceutil::Event
{
public:
    FocusChangedEvent( int focus ) :
        Event( FocusChanged ),
        focus_(focus) {};

    int focus_;
};
typedef IceUtil::Handle<FocusChangedEvent> FocusChangedEventPtr;

class NewTraceMessageEvent : public hydroiceutil::Event
{
public:
    NewTraceMessageEvent( const orca::TracerData & data ) :
        Event( NewTraceMessage ),
        data_(data)  {};

    orca::TracerData data_;
};
typedef IceUtil::Handle<NewTraceMessageEvent> NewTraceMessageEventPtr;

class NewLocalTraceEvent : public hydroiceutil::Event
{
public:
    NewLocalTraceEvent( const std::string& msg ) :
        Event( NewLocalTrace ),
        msg_(msg)  {};

    std::string msg_;
};
typedef IceUtil::Handle<NewLocalTraceEvent> NewLocalTraceEventPtr;

class PreviousFieldEvent : public hydroiceutil::Event
{
public:
    PreviousFieldEvent() :
        Event( PreviousField ) {};
};

class NextFieldEvent : public hydroiceutil::Event
{
public:
    NextFieldEvent() :
        Event( NextField ) {};
};

class ValueUpEvent : public hydroiceutil::Event
{
public:
    ValueUpEvent() :
        Event( ValueUp ) {};
};

class ValueDownEvent : public hydroiceutil::Event
{
public:
    ValueDownEvent() :
        Event( ValueDown ) {};
};

class ActionEvent : public hydroiceutil::Event
{
public:
    ActionEvent() :
        Event( Action ) {};
};

} // namespace

#endif
