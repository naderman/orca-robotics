/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCAPROBEFACTORY_STATUS_INTERFACE_PROBE_H
#define ORCA2_ORCAPROBEFACTORY_STATUS_INTERFACE_PROBE_H

#include <orcaprobe/interfaceprobe.h>
#include <orca/status.h>

namespace orcaprobefactory
{

class StatusProbe : public orca::StatusConsumer, public orcaprobe::InterfaceProbe
{

public:

    StatusProbe( const orca::FQInterfaceName& name, orcaprobe::DisplayDriver& display,
                                const orcaice::Context& context );

    virtual int loadOperationEvent( const int index, orcacm::OperationData& data );

    // from consumer
    virtual void setData(const orca::StatusData& data, const Ice::Current&);
    
private:

    int loadGetData( orcacm::OperationData& data );
    int loadSubscribe( orcacm::OperationData& data );
    int loadUnsubscribe( orcacm::OperationData& data );

};

} // namespace

#endif
