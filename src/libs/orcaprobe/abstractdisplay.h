/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCAPROBE_ABSTRACT_DISPLAY_H
#define ORCA_ORCAPROBE_ABSTRACT_DISPLAY_H

#include <orcacm/types.h>

namespace orcaprobe
{

class IBrowser;

class AbstractDisplay
{

public:

    enum FocusType
    {
        RegistryFocus=0,
        PlatformFocus,
        ComponentFocus,
        InterfaceFocus,
        OperationFocus,
        ResultFocus
    };

    virtual ~AbstractDisplay() {};

    // need a separate enable function because the Qt driver does not return
    virtual void enable( IBrowser* browser )=0;

    virtual void showNetworkActivity( bool isActive )=0;

    virtual void setRegistryData( const orcacm::RegistryHierarchicalData1 & data )=0;

    virtual void setPlatformData( const orcacm::RegistryHierarchicalData2 & data )=0;
    
    virtual void setComponentData( const orcacm::ComponentData & data )=0;
    
    virtual void setInterfaceData( const orcacm::InterfaceData & data )=0;
    
    virtual void setOperationData( const orcacm::OperationData & data )=0;

    virtual void setFocus( FocusType focus )=0;
};

} // namespace

#endif
