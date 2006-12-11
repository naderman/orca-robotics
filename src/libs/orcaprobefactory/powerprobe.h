/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCAPROBEFACTORY_POWER_INTERFACE_PROBE_H
#define ORCA2_ORCAPROBEFACTORY_POWER_INTERFACE_PROBE_H

#include <orcaprobe/interfaceprobe.h>
#include <orca/power.h>

namespace orcaprobefactory
{

class PowerProbe : public orca::PowerConsumer, public orcaprobe::InterfaceProbe
{

public:

    PowerProbe( const orca::FQInterfaceName & name, orcaprobe::DisplayDriver & display,
                                const orcaice::Context & context );

    virtual int loadOperationEvent( const int index, orcacm::OperationData & data );
    
    virtual void setData(const orca::PowerDataPtr& data, const Ice::Current&);

private:

    int loadGetData( orcacm::OperationData & data );
    int loadSubscribe( orcacm::OperationData & data );
    int loadUnsubscribe( orcacm::OperationData & data );

};

} // namespace

#endif
