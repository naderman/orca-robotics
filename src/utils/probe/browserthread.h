/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2006-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef BROWSER_HANDLER_H
#define BROWSER_HANDLER_H

#include <vector>
#include <hydroiceutil/eventqueue.h>
#include <orcaice/context.h>
#include <orcacm/types.h>

#include "browserfsm.h"
#include <orcaprobe/ibrowser.h>

namespace orcaprobe
{
    class orcaprobe::IDisplay;
}

namespace probe
{

class ProbeFactory;
class InterfaceProbe;

class BrowserThread : public orcaprobe::IBrowser, public gbxsickacfr::gbxiceutilacfr::SafeThread, public BrowserFsm
{

public:
    BrowserThread( orcaprobe::IDisplay & display,
                    std::vector<orcaprobe::Factory*> &factories,
                    const orcaice::Context & context );
    virtual ~BrowserThread();

    // from orcaprobe::IBrowser
    virtual void chooseActivate();
    virtual void chooseReload();
    virtual void chooseUp();
    virtual void chooseTop();
    virtual void choosePick( int );
    virtual void chooseFilter( const std::string & );
    virtual void chooseDeactivate();

private:

    // from Thread
    virtual void walk();

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

    std::vector<orcaprobe::Factory*> &factories_;

    orcaprobe::IDisplay & display_;

    hydroiceutil::EventQueuePtr events_;
 
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
