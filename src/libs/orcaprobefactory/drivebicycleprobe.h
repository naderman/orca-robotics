/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCAPROBEFACTORY_DRIVEBICYCLE_INTERFACE_PROBE_H
#define ORCA_ORCAPROBEFACTORY_DRIVEBICYCLE_INTERFACE_PROBE_H

#include <orcaprobe/interfaceprobe.h>
#include <orcaifaceimpl/drivebicycle.h>

namespace orcaprobefactory
{

class DriveBicycleProbe : public orcaprobe::InterfaceProbe
{

public:

    DriveBicycleProbe( const orca::FQInterfaceName& name, const Ice::ObjectPrx& adminPrx, orcaprobe::AbstractDisplay& display,
                                const orcaice::Context& context );

    virtual int loadOperationEvent( const int index, orcacm::OperationData& data );

private:

    int loadGetDescription( orcacm::OperationData& data );
    int loadGetData( orcacm::OperationData& data );
    int loadSubscribe( orcacm::OperationData& data );
    int loadUnsubscribe( orcacm::OperationData& data );
    int loadSetData( orcacm::OperationData& data );

    orcaifaceimpl::PrintingDriveBicycleConsumerImplPtr consumer_;
};

} // namespace

#endif
