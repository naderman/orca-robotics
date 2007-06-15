/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2006-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_PROBE_GUI_QT_DISPLAY_EVENTS_H
#define ORCA2_PROBE_GUI_QT_DISPLAY_EVENTS_H

#include <QEvent>
#include <orcacm/types.h>
#include "../idisplay.h" // for focus types

namespace probe
{

enum EventType
{
    NetworkActivityChanged=QEvent::User+1,
    FocusChanged,
    RegistryDataChanged,
    PlatformDataChanged,
    ComponentDataChanged,
    InterfaceDataChanged,
    OperationDataChanged
};

class QorcaNetworkActivityChangedEvent : public QEvent
{
public:
    QorcaNetworkActivityChangedEvent( bool isActive )
        : QEvent( QEvent::Type(NetworkActivityChanged) ),
          isActive_(isActive) {};

    bool isActive_;
};

class QorcaFocusChangedEvent : public QEvent
{
public:
    QorcaFocusChangedEvent( IDisplay::FocusType focus )
        : QEvent( QEvent::Type(FocusChanged) ),
          focus_(focus)  {};

    IDisplay::FocusType focus_;
};

class QorcaRegistryDataChangedEvent : public QEvent
{
public:
    QorcaRegistryDataChangedEvent( const orcacm::RegistryHierarchicalData1 & data )
        : QEvent( QEvent::Type(RegistryDataChanged) ),
          data_(data) {};

    orcacm::RegistryHierarchicalData1 data_;
};

class QorcaPlatformDataChangedEvent : public QEvent
{
public:
    QorcaPlatformDataChangedEvent( const orcacm::RegistryHierarchicalData2 & data )
        : QEvent( QEvent::Type(PlatformDataChanged) ),
          data_(data) {};

    orcacm::RegistryHierarchicalData2 data_;
};

class QorcaComponentDataChangedEvent : public QEvent
{
public:
    QorcaComponentDataChangedEvent( const orcacm::ComponentData & data )
        : QEvent( QEvent::Type(ComponentDataChanged) ),
          data_(data) {};

    orcacm::ComponentData data_;
};

class QorcaInterfaceDataChangedEvent : public QEvent
{
public:
    QorcaInterfaceDataChangedEvent( const orcacm::InterfaceData & data )
        : QEvent( QEvent::Type(InterfaceDataChanged) ),
          data_(data) {};

    orcacm::InterfaceData data_;
};

class QorcaOperationDataChangedEvent : public QEvent
{
public:
    QorcaOperationDataChangedEvent( const orcacm::OperationData & data )
        : QEvent( QEvent::Type(OperationDataChanged) ),
          data_(data) {};

    orcacm::OperationData data_;
};

} // namespace

#endif
