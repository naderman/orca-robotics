/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_LOGPLAYER_COMPONENT_H
#define ORCA2_LOGPLAYER_COMPONENT_H

#include <vector>
#include <string>
#include <orcaice/component.h>
#include <hydrodll/dll.h>
#include "interfacetypecounter.h"

namespace orcalog
{
    class MasterFileReader;
    class Replayer;
    class ReplayerFactory;
}

namespace logplayer
{

class MainLoop;

class Component : public orcaice::Component
{
public:

    Component( const std::string& compName );
    virtual ~Component();

    // component interface
    virtual void start();
    virtual void stop();

private:
  
    // utilities
    void loadPluginLibraries( const std::string& factoryLibNames );
    void createReplayer( const std::string & interfaceType, 
                         const std::string& format,
                         const std::string& filename,
                         bool               enabled,
                         bool               require );

    MainLoop *mainLoop_;
    
    std::vector<orcalog::ReplayerFactory*>           replayerFactories_;
    std::vector<hydrodll::DynamicallyLoadedLibrary*> libraries_;

    orcalog::MasterFileReader *masterFileReader_;
    std::vector<orcalog::Replayer*> replayers_;

    // Ensure interfaces of a given type are unique.
    InterfaceTypeCounter interfaceTypeCounter_;
    
};

} // namespace

#endif
