/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCAPROBEFACTORY_LOCALISE2D_INTERFACE_PROBE_H
#define ORCA_ORCAPROBEFACTORY_LOCALISE2D_INTERFACE_PROBE_H

#include <orcaprobe/interfaceprobe.h>
#include <orca/localise2d.h>

namespace orcaprobefactory
{

class Localise2dProbe : public orca::Localise2dConsumer, public orcaprobe::InterfaceProbe
{

public:

    Localise2dProbe( const orca::FQInterfaceName& name, orcaprobe::IDisplay& display,
                                const orcaice::Context& context );

    virtual int loadOperationEvent( const int index, orcacm::OperationData& data );
    
    virtual void setData(const orca::Localise2dData& data, const Ice::Current&);

private:

    int loadGetDescription( orcacm::OperationData& data );
    int loadGetData( orcacm::OperationData& data );
    int loadGetVehicleGeometry( orcacm::OperationData& data );
    int loadSubscribe( orcacm::OperationData& data );
    int loadUnsubscribe( orcacm::OperationData& data );

    orca::Localise2dConsumerPrx callbackPrx_;
    orcacm::OperationData subscribeOperationData_;
};

} // namespace

#endif
