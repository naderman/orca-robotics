/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCAPROBEFACTORY_ODOMETRY2D_INTERFACE_PROBE_H
#define ORCA2_ORCAPROBEFACTORY_ODOMETRY2D_INTERFACE_PROBE_H

#include <orcaprobe/interfaceprobe.h>
#include <orca/odometry2d.h>

namespace orcaprobefactory
{

class Odometry2dProbe : public orca::Odometry2dConsumer, public orcaprobe::InterfaceProbe
{

public:

    Odometry2dProbe( const orca::FQInterfaceName& name, orcaprobe::IDisplay& display,
                                const orcaice::Context& context );

    virtual int loadOperationEvent( const int index, orcacm::OperationData& data );
    
    virtual void setData(const orca::Odometry2dData& data, const Ice::Current&);

private:

    int loadGetDescription( orcacm::OperationData& data );
    int loadGetData( orcacm::OperationData& data );
    int loadSubscribe( orcacm::OperationData& data );
    int loadUnsubscribe( orcacm::OperationData& data );

    orca::Odometry2dConsumerPrx callbackPrx_;
    orcacm::OperationData subscribeOperationData_;

};

} // namespace

#endif
