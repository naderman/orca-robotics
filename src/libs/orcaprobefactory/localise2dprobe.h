/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCAPROBEFACTORY_LOCALISE2D_INTERFACE_PROBE_H
#define ORCA_ORCAPROBEFACTORY_LOCALISE2D_INTERFACE_PROBE_H

#include <orcaprobe/interfaceprobe.h>
#include <orca/localise2d.h>
#include <orcaifaceimpl/printingconsumers.h>

namespace orcaprobefactory
{

class Localise2dProbe : public orcaprobe::InterfaceProbe
{

public:

    Localise2dProbe( const orca::FQInterfaceName& name, const Ice::ObjectPrx& adminPrx, orcaprobe::AbstractDisplay& display,
                                const orcaice::Context& context );
    ~Localise2dProbe();

    virtual int loadOperationEvent( const int index, orcacm::OperationData& data );

private:

    int loadGetDescription( orcacm::OperationData& data );
    int loadGetData( orcacm::OperationData& data );
    int loadGetVehicleGeometry( orcacm::OperationData& data );
    int loadSubscribe( orcacm::OperationData& data );
    int loadUnsubscribe( orcacm::OperationData& data );

    orcaifaceimpl::PrintingLocalise2dConsumerImplPtr consumer_;
};

} // namespace

#endif
