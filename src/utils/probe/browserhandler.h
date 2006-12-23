/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2006-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_PROBE_BROWSER_HANDLER_H
#define ORCA2_PROBE_BROWSER_HANDLER_H

#include <vector>
#include <orcaice/eventqueue.h>
#include <orcaice/context.h>
#include <orcacm/types.h>

#include "browserfsm.h"

namespace probe
{

class ProbeFactory;
class InterfaceProbe;

class BrowserHandler : public orcaice::Thread, public BrowserFsm
{

public:
    BrowserHandler( const orcaice::EventQueuePtr & myQueue, 
                    const orcaice::EventQueuePtr & otherQueue, 
                    std::vector<orcaprobe::Factory*> &factories,
                    const orcaice::Context & context );
    virtual ~BrowserHandler();

private:

    // from Thread
    virtual void run();

    // from BrowserFsm
    virtual void loadRegistry();
    virtual void filterRegistry();
    virtual void showRegistry();
    virtual void loadPlatform();
    virtual void showPlatform();
    virtual void loadComponent();
    virtual void showComponent();
    virtual void loadInterface();
    virtual void showInterface();
    virtual void loadOperation();
    virtual void showOperation();
    
    virtual void pickLastPlatform()  { pick_=lastPlatformPick_;};
    virtual void pickLastComponent() { pick_=lastComponentPick_;};
    virtual void pickLastInterface() { pick_=lastInterfacePick_;};
    virtual void pickLastOperation() { pick_=lastOperationPick_;};
    
    virtual void quit();

    orcaice::EventQueuePtr myQueue_;
    orcaice::EventQueuePtr otherQueue_;
    std::vector<orcaprobe::Factory*> &factories_;

    orcaprobe::InterfaceProbe* ifaceProbe_;
    orcaice::Context context_;

    // user's last choice
    int pick_;
    int lastPlatformPick_;
    int lastComponentPick_;
    int lastInterfacePick_;
    int lastOperationPick_;

    orcacm::RegistryHomeData registryHomeData_;

    orcacm::RegistryHierarchicalData1 registryData_;
    orcacm::RegistryHierarchicalData2 platformData_;
    orcacm::ComponentData componentData_;
    orcacm::InterfaceData interfaceData_;
    orcacm::OperationData operationData_;

    // user's last filter
    std::string filter_;
};

} // namespace

#endif
