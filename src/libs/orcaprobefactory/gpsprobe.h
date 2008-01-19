/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alexei Makarenko, Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCAPROBEFACTORY_GPS_INTERFACE_PROBE_H
#define ORCA_ORCAPROBEFACTORY_GPS_INTERFACE_PROBE_H

#include <orcaprobe/interfaceprobe.h>
#include <orca/gps.h>
#include <orcaprobefactory/genericgpsprobe.h>

namespace orcaprobefactory
{

    typedef GenericGpsProbe<orca::GpsConsumer,
                            orca::GpsConsumerPrx,
                            orca::GpsPrx,
                            orca::GpsData,
                            orca::GpsDescription>  GpsProbe;

} // namespace

#endif
