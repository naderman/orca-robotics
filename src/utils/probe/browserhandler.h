/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_PROBE_BROWSER_HANDLER_H
#define ORCA2_PROBE_BROWSER_HANDLER_H

#include <vector>
#include <orcaice/buffer.h>
#include <orcaice/context.h>
#include <orcacm/types.h>

#include "browserfsm.h"
#include "browserdriver.h"

namespace probe
{

class ProbeFactory;
class DisplayDriver;
class InterfaceProbe;

class BrowserHandler : public orcaice::Thread, public BrowserFsm
{

public:
    BrowserHandler( orcaice::Buffer<BrowserEvent> & eventPipe,
                    ProbeFactory & probeFactory,
                    DisplayDriver & display,
                    const orcaice::Context & context );
    virtual ~BrowserHandler();

    virtual void run();

    virtual void loadRegistry();
    virtual void loadComponent();
    virtual void loadInterface();
    virtual void loadOperation();
    
    virtual void pickLastComponent() { pick_=lastComponentPick_;};
    virtual void pickLastInterface() { pick_=lastInterfacePick_;};
    virtual void pickLastOperation() { pick_=lastOperationPick_;};
    
    virtual void quit();

private:

    orcaice::Buffer<BrowserEvent> & eventPipe_;

    ProbeFactory & probeFactory_;

    DisplayDriver & displayDriver_;
    
    InterfaceProbe* ifaceProbe_;

    orcaice::Context context_;

    int pick_;
    int lastComponentPick_;
    int lastInterfacePick_;
    int lastOperationPick_;
    orcacm::RegistryData registryData_;
    orcacm::ComponentData componentData_;
    orcacm::InterfaceData interfaceData_;
    orcacm::OperationData operationData_;

};

} // namespace

#endif
