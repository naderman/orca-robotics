/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCAPROBEFACTORY_HOME_INTERFACE_PROBE_H
#define ORCA_ORCAPROBEFACTORY_HOME_INTERFACE_PROBE_H

#include <orcaprobe/interfaceprobe.h>

namespace orcaprobefactory
{

class HomeProbe : public orcaprobe::InterfaceProbe
{

public:

    HomeProbe( const orca::FQInterfaceName& name, const Ice::ObjectPrx& adminPrx, orcaprobe::AbstractDisplay& display,
                                const orcaice::Context& context );

    virtual int loadOperationEvent( const int index, orcacm::OperationData& data );
    
private:
        
    int loadGetData( orcacm::OperationData& data );
};

} // namespace

#endif
