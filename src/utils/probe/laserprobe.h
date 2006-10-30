/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_PROBE_LASER_INTERFACE_PROBE_H
#define ORCA2_PROBE_LASER_INTERFACE_PROBE_H

#include "interfaceprobe.h"
#include <orca/laser.h>

namespace probe
{

class LaserProbe : public InterfaceProbe
{

public:

    LaserProbe( const orca::FQInterfaceName & name, DisplayDriver & display,
                                const orcaice::Context & context );

    virtual int loadOperation( const int index );
    
private:

    int loadGetData();
    int loadGetConfig();
    int loadGetGeometry();
//     int loadSetConfig();
    int loadSubscribe();
    int loadUnsubscribe();
    
//     orca::RangeScannerConsumer consumerPrx_;

};

} // namespace

#endif
