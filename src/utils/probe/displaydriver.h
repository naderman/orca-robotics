/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_BORCA_DISPLAY_DRIVER_INTERFACE_H
#define ORCA2_BORCA_DISPLAY_DRIVER_INTERFACE_H

#include <orcacm/types.h>
#include "browserdriver.h"

namespace probe
{

class DisplayDriver
{

public:

    virtual ~DisplayDriver() {};

    virtual void showRegistryData( const orcacm::RegistryData & data )=0;
    
    virtual void showComponentData( const orcacm::ComponentData & data )=0;
    
    virtual void showInterfaceData( const orcacm::InterfaceData & data )=0;
    
    virtual void showOperationData( const orcacm::OperationData & data )=0;

};

} // namespace

#endif
