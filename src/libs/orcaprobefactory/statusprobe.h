/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCAPROBEFACTORY_STATUS_INTERFACE_PROBE_H
#define ORCA_ORCAPROBEFACTORY_STATUS_INTERFACE_PROBE_H

#include <orcaprobe/interfaceprobe.h>
#include <orca/status.h>
#include <orcaifaceimpl/printingconsumers.h>
// #include <orcaifaceimpl/consumermanager.h>

namespace orcaprobefactory
{

class StatusProbe : public orcaprobe::InterfaceProbe
//         public orcaifaceimpl::SimpleConsumerImpl<orca::StatusPrx,orca::StatusConsumer,orca::StatusConsumerPrx,orca::StatusData>
{

public:

    StatusProbe( const orca::FQInterfaceName& name, orcaprobe::AbstractDisplay& display,
                                const orcaice::Context& context );

    virtual int loadOperationEvent( const int index, orcacm::OperationData& data );
    
private:

    int loadGetData( orcacm::OperationData& data );
    int loadSubscribe( orcacm::OperationData& data );
    int loadUnsubscribe( orcacm::OperationData& data );

    orcaifaceimpl::PrintingStatusConsumerImplPtr consumer_;
//     orcaifaceimpl::ConsumerManager consumerManager_;
};

} // namespace

#endif
