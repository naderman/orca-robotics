/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCAPROBEFACTORY_BINARYSWITCH_INTERFACE_PROBE_H
#define ORCA2_ORCAPROBEFACTORY_BINARYSWITCH_INTERFACE_PROBE_H

#include <orcaprobe/interfaceprobe.h>
#include <orca/binaryswitch.h>

namespace orcaprobefactory
{

class BinarySwitchProbe : public orcaprobe::InterfaceProbe
{

public:

    BinarySwitchProbe( const orca::FQInterfaceName & name, orcaprobe::DisplayDriver & display,
                                const orcaice::Context & context );

    virtual int loadOperationEvent( const int index, orcacm::OperationData & data );
    
private:

    int loadGetData();
    int loadSetState();
    int loadTimedSetState();
    int loadToggleState();
    int loadTimedToggleState();
};

} // namespace

#endif
