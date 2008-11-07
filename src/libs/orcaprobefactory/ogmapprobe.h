/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCAPROBEFACTORY_OGMAP_INTERFACE_PROBE_H
#define ORCA_ORCAPROBEFACTORY_OGMAP_INTERFACE_PROBE_H

#include <orcaprobe/interfaceprobe.h>
#include <orca/ogmap.h>
#include <orcaifaceimpl/printingconsumers.h>

namespace orcaprobefactory
{

class OgMapProbe : public orcaprobe::InterfaceProbe
{

public:

    OgMapProbe( const orca::FQInterfaceName& name, const Ice::ObjectPrx& adminPrx, orcaprobe::AbstractDisplay& display,
                                const orcaice::Context& context );
    ~OgMapProbe();

    virtual int loadOperationEvent( const int index, orcacm::OperationData & data );

private:

    int loadGetData( orcacm::OperationData & data );
    int loadSubscribe( orcacm::OperationData & data );
    int loadUnsubscribe( orcacm::OperationData & data );

    orcaifaceimpl::PrintingOgMapConsumerImplPtr consumer_;
};

} // namespace

#endif
