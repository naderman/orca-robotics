/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCAPROBEFACTORY_DRIVEBICYCLE_INTERFACE_PROBE_H
#define ORCA2_ORCAPROBEFACTORY_DRIVEBICYCLE_INTERFACE_PROBE_H

#include <orcaprobe/interfaceprobe.h>
#include <orca/drivebicycle.h>

namespace orcaprobefactory
{

class DriveBicycleProbe : public orca::DriveBicycleConsumer, public orcaprobe::InterfaceProbe
{

public:

    DriveBicycleProbe( const orca::FQInterfaceName& name, orcaprobe::DisplayDriver& display,
                                const orcaice::Context& context );

    virtual int loadOperationEvent( const int index, orcacm::OperationData& data );
    
    virtual void setData(const orca::DriveBicycleData& data, const Ice::Current&);

private:

    int loadGetDescription( orcacm::OperationData& data );
    int loadGetData( orcacm::OperationData& data );
    int loadSubscribe( orcacm::OperationData& data );
    int loadUnsubscribe( orcacm::OperationData& data );

    orca::DriveBicycleConsumerPrx callbackPrx_;
    orcacm::OperationData subscribeOperationData_;

};

} // namespace

#endif