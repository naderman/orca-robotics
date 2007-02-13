/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alexei Makarenko, Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCAPROBEFACTORY_GPS_INTERFACE_PROBE_H
#define ORCA2_ORCAPROBEFACTORY_GPS_INTERFACE_PROBE_H

#include <orcaprobe/interfaceprobe.h>
#include <orca/gps.h>

namespace orcaprobefactory
{

class GpsProbe : public orca::GpsConsumer, public orcaprobe::InterfaceProbe
{

public:

    GpsProbe( const orca::FQInterfaceName & name,
              orcaprobe::DisplayDriver & display,              
              const orcaice::Context & context );

    virtual int loadOperationEvent( const int index, orcacm::OperationData & data );
    
    virtual void setData(const orca::GpsData & data, const Ice::Current&);

private:

    int loadGetDescription( orcacm::OperationData & data );
    int loadGetData(        orcacm::OperationData & data );
    int loadGetTimeData(    orcacm::OperationData & data );
    int loadGetMapGridData( orcacm::OperationData & data );
    int loadSubscribe(      orcacm::OperationData & data );
    int loadUnsubscribe(    orcacm::OperationData & data );

};

} // namespace

#endif
