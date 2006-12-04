/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */


#ifndef ORCA2_PROBE_COMPONENT_H
#define ORCA2_PROBE_COMPONENT_H

#include <vector>
#include <string>
#include <orcaice/component.h>
#include <orcadynamicload/dynamicload.h>

namespace orcaprobe
{
    class Factory;
}

namespace probe
{

class Component : public orcaice::Component
{
public:
    Component();
    virtual ~Component();

    // component interface
    virtual void start();
    virtual void stop();

private:
    std::vector<orcaprobe::Factory*>                  factories_;
    std::vector<orcadynamicload::DynamicallyLoadedLibrary*> libraries_;

    // utilities
    void loadPluginLibraries( const std::string & factoryLibNames );
};

} // namespace

#endif
