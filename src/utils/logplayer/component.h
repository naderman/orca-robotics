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
#include <orcadynamicload/dynamicload.h>

namespace orcalog
{
    class ReplayMaster;
    class Replayer;
    class ReplayFactory;
}

namespace logplayer
{

class MainLoop;

class Component : public orcaice::Component
{
public:

    Component( const std::string & compName );
    virtual ~Component();

    // component interface
    virtual void start();
    virtual void stop();

private:
  
    MainLoop *mainLoop_;
    
    std::vector<orcalog::ReplayFactory*>                    replayFactories_;
    std::vector<orcadynamicload::DynamicallyLoadedLibrary*> libraries_;

    orcalog::ReplayMaster *master_;
    std::vector<orcalog::Replayer*> replayers_;
    
    // utilities
    void loadPluginLibraries( const std::string & factoryLibNames );
    void createReplayer( const std::string    &interfaceType, 
                   const std::string  &format,
                   const std::string  &filename );
};

} // namespace

#endif
