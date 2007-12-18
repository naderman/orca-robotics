/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCAPROBEFACTORY_BINARYSWITCH_INTERFACE_PROBE_H
#define ORCA_ORCAPROBEFACTORY_BINARYSWITCH_INTERFACE_PROBE_H

#include <orcaprobe/interfaceprobe.h>
#include <orca/binaryswitch.h>

namespace orcaprobefactory
{

class BinarySwitchProbe : public orcaprobe::InterfaceProbe
{

public:

    BinarySwitchProbe( const orca::FQInterfaceName & name, orcaprobe::IDisplay & display,
                                const orcaice::Context & context );

    virtual int loadOperationEvent( const int index, orcacm::OperationData & data );
    
private:

    int loadGetData( orcacm::OperationData & data );
    int loadSetState( orcacm::OperationData & data );
    int loadTimedSetState( orcacm::OperationData & data );
    int loadToggleState( orcacm::OperationData & data );
    int loadTimedToggleState( orcacm::OperationData & data );
};

} // namespace

#endif
