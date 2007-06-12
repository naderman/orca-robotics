/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_ORCAPROBEFACTORY_LOCALISE3D_INTERFACE_PROBE_H
#define ORCA2_ORCAPROBEFACTORY_LOCALISE3D_INTERFACE_PROBE_H

#include <orcaprobe/interfaceprobe.h>
#include <orca/localise3d.h>

namespace orcaprobefactory
{

class Localise3dProbe : public orca::Localise3dConsumer, public orcaprobe::InterfaceProbe
{

public:

    Localise3dProbe( const orca::FQInterfaceName& name, orcaprobe::DisplayDriver& display,
                                const orcaice::Context& context );

    virtual int loadOperationEvent( const int index, orcacm::OperationData& data );
    
    virtual void setData(const orca::Localise3dData& data, const Ice::Current&);

private:

    int loadGetDescription( orcacm::OperationData& data );
    int loadGetData( orcacm::OperationData& data );
    int loadGetDataAtTime( orcacm::OperationData& data );
    int loadSubscribe( orcacm::OperationData& data );
    int loadUnsubscribe( orcacm::OperationData& data );

    orca::Localise3dConsumerPrx callbackPrx_;
    orcacm::OperationData subscribeOperationData_;
};

} // namespace

#endif
