/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_ORCAPROBEFACTORY_BUTTON_INTERFACE_PROBE_H
#define ORCA_ORCAPROBEFACTORY_BUTTON_INTERFACE_PROBE_H

#include <orcaprobe/interfaceprobe.h>
#include <orca/button.h>

namespace orcaprobefactory
{

class ButtonProbe : public orcaprobe::InterfaceProbe
{

public:

    ButtonProbe( const orca::FQInterfaceName& name, orcaprobe::AbstractDisplay& display,
               const orcaice::Context& context );

    virtual int loadOperationEvent( const int index, orcacm::OperationData& data );

private:

    int loadPress( orcacm::OperationData& data );

    orcacm::OperationData subscribeOperationData_;
};

} // namespace

#endif
