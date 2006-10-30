/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_PROBE_POWER_INTERFACE_PROBE_H
#define ORCA2_PROBE_POWER_INTERFACE_PROBE_H

#include "interfaceprobe.h"
#include <orca/power.h>

namespace probe
{

class PowerProbe : public InterfaceProbe
{

public:

    PowerProbe( const orca::FQInterfaceName & name, DisplayDriver & display,
                                const orcaice::Context & context );

    virtual int loadOperation( const int index );
    
private:

    int loadGetData();
    int loadSubscribe();
    int loadUnsubscribe();
    
    orca::PowerConsumerPrx consumerPrx_;

};

} // namespace

#endif
