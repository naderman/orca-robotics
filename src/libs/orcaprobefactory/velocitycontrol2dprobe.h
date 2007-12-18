/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCAPROBEFACTORY_VELOCITYCONTROL2D_INTERFACE_PROBE_H
#define ORCA_ORCAPROBEFACTORY_VELOCITYCONTROL2D_INTERFACE_PROBE_H

#include <orcaprobe/interfaceprobe.h>
#include <orca/velocitycontrol2d.h>

namespace orcaprobefactory
{

class VelocityControl2dProbe : public orcaprobe::InterfaceProbe
{

public:

    VelocityControl2dProbe( const orca::FQInterfaceName &name, 
                            orcaprobe::IDisplay         &display,
                            const orcaice::Context      &context );

    virtual int loadOperationEvent( const int index, orcacm::OperationData& data );
    
private:

    int loadGetDescription( orcacm::OperationData& data );

    orcacm::OperationData subscribeOperationData_;

};

} // namespace

#endif
