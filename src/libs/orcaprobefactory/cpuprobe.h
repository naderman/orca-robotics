/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCAPROBEFACTORY_CPU_INTERFACE_PROBE_H
#define ORCA2_ORCAPROBEFACTORY_CPU_INTERFACE_PROBE_H

#include <orcaprobe/interfaceprobe.h>
#include <orca/cpu.h>

namespace orcaprobefactory
{

class CpuProbe : public orcaprobe::InterfaceProbe
{

public:

    CpuProbe( const orca::FQInterfaceName & name, orcaprobe::IDisplay & display,
                                const orcaice::Context & context );

    virtual int loadOperationEvent( const int index, orcacm::OperationData & data );
    
private:

    int loadGetInfo( orcacm::OperationData& data );
    int loadGetData( orcacm::OperationData& data );
};

} // namespace

#endif
