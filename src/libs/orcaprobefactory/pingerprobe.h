/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCAPROBEFACTORY_PINGER_INTERFACE_PROBE_H
#define ORCA_ORCAPROBEFACTORY_PINGER_INTERFACE_PROBE_H

#include <orcaprobe/interfaceprobe.h>
#include <orca/pinger.h>

namespace orcaprobefactory
{

class PingerProbe : public orcaprobe::InterfaceProbe
{

public:

    PingerProbe( const orca::FQInterfaceName& name, const Ice::ObjectPrx& adminPrx, orcaprobe::AbstractDisplay& display,
                                const orcaice::Context & context );

    virtual int loadOperationEvent( const int index, orcacm::OperationData & data );
    
private:

    int loadPing( orcacm::OperationData & data );
};

} // namespace

#endif
