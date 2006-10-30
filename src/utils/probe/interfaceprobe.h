/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_PROBE_INTERFACE_PROBE_H
#define ORCA2_PROBE_INTERFACE_PROBE_H

#include <orca/orca.h>
#include <orcacm/types.h>
#include <orcaice/context.h>

namespace probe
{

class DisplayDriver;

class InterfaceProbe
{

public:
    InterfaceProbe( const orca::FQInterfaceName & name, DisplayDriver & display,
                                const orcaice::Context & context );
        
    virtual ~InterfaceProbe() {};

    std::vector<orcacm::OperationHeader> operations();
    
    orcacm::OperationData getOperationData( const int index );

    // returns 0 if all good, 1 on error
    virtual int loadOperation( const int index )=0;

protected:

    orca::FQInterfaceName name_;
    std::string id_;
    std::vector<std::string> operations_;
    
    DisplayDriver & displayDriver_;
    orcaice::Context context_;
    Ice::ObjectPrx prx_;
};

} // namespace

#endif
