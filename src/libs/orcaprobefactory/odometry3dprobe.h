/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCAPROBEFACTORY_ODOMETRY3D_INTERFACE_PROBE_H
#define ORCA_ORCAPROBEFACTORY_ODOMETRY3D_INTERFACE_PROBE_H

#include <orcaprobe/interfaceprobe.h>
#include <orca/odometry3d.h>

namespace orcaprobefactory
{

class Odometry3dProbe : public orca::Odometry3dConsumer, public orcaprobe::InterfaceProbe
{

public:

    Odometry3dProbe( const orca::FQInterfaceName& name, orcaprobe::IDisplay& display,
                                const orcaice::Context& context );

    virtual int loadOperationEvent( const int index, orcacm::OperationData& data );
    
    virtual void setData(const orca::Odometry3dData& data, const Ice::Current&);

private:

    int loadGetDescription( orcacm::OperationData& data );
    int loadGetData( orcacm::OperationData& data );
    int loadSubscribe( orcacm::OperationData& data );
    int loadUnsubscribe( orcacm::OperationData& data );

};

} // namespace

#endif
