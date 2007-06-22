/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_PROBE_TERM_DISPLAY_EVENTS_H
#define ORCA2_PROBE_TERM_DISPLAY_EVENTS_H

#include <orcaice/eventqueue.h>
#include <orcacm/types.h>
#include <orcaprobe/idisplay.h> // for focus types

// alexm: we put these event definitions on the top level (not with term-iostream driver)
// in case we write another terminal display driver (e.g. ncurses) which is likely to 
// use the same events.

namespace probe
{

enum EventType  
{      
    NetworkActivityChanged=0,
    FocusChanged,
    RegistryDataChanged,
    PlatformDataChanged,
    ComponentDataChanged,
    InterfaceDataChanged,
    OperationDataChanged
};

class NetworkActivityChangedEvent : public orcaice::Event
{
public:
    NetworkActivityChangedEvent( bool isActive )
        : Event( NetworkActivityChanged ),
          isActive_(isActive) {};

    bool isActive_;
};
typedef IceUtil::Handle<NetworkActivityChangedEvent> NetworkActivityChangedEventPtr;

class FocusChangedEvent : public orcaice::Event
{
public:
    FocusChangedEvent( orcaprobe::IDisplay::FocusType focus )
        : Event( FocusChanged ),
          focus_(focus)  {};

    orcaprobe::IDisplay::FocusType focus_;
};
typedef IceUtil::Handle<FocusChangedEvent> FocusChangedEventPtr;

class RegistryDataChangedEvent : public orcaice::Event
{
public:
    RegistryDataChangedEvent( const orcacm::RegistryHierarchicalData1 & data )
        : Event( RegistryDataChanged ),
          data_(data) {};

    orcacm::RegistryHierarchicalData1 data_;
};
typedef IceUtil::Handle<RegistryDataChangedEvent> RegistryDataChangedEventPtr;

class PlatformDataChangedEvent : public orcaice::Event
{
public:
    PlatformDataChangedEvent( const orcacm::RegistryHierarchicalData2 & data )
        : Event( PlatformDataChanged ),
          data_(data) {};

    orcacm::RegistryHierarchicalData2 data_;
};
typedef IceUtil::Handle<PlatformDataChangedEvent> PlatformDataChangedEventPtr;

class ComponentDataChangedEvent : public orcaice::Event
{
public:
    ComponentDataChangedEvent( const orcacm::ComponentData & data )
        : Event( ComponentDataChanged ),
          data_(data) {};

    orcacm::ComponentData data_;
};
typedef IceUtil::Handle<ComponentDataChangedEvent> ComponentDataChangedEventPtr;

class InterfaceDataChangedEvent : public orcaice::Event
{
public:
    InterfaceDataChangedEvent( const orcacm::InterfaceData & data )
        : Event( InterfaceDataChanged ),
          data_(data) {};

    orcacm::InterfaceData data_;
};
typedef IceUtil::Handle<InterfaceDataChangedEvent> InterfaceDataChangedEventPtr;


class OperationDataChangedEvent : public orcaice::Event
{
public:
    OperationDataChangedEvent( const orcacm::OperationData & data )
        : Event( OperationDataChanged ),
          data_(data) {};

    orcacm::OperationData data_;
};
typedef IceUtil::Handle<OperationDataChangedEvent> OperationDataChangedEventPtr;



} // namespace

#endif
