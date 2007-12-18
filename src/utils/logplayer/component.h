/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef COMPONENT_H
#define COMPONENT_H

#include <vector>
#include <string>
#include <orcaice/component.h>
#include <hydrodll/dll.h>
#include "interfacetypecounter.h"
#include <hydroutil/safethread.h>

namespace orcalog
{
    class MasterFileReader;
    class Replayer;
    class ReplayerFactory;
}

namespace logplayer
{

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

    void readReplayParams( IceUtil::Time &beginTime, double &replayRate, bool &autoStart );

    hydroutil::ThreadPtr       replayConductor_;
    hydroutil::ThreadPtr       highLevelController_;

    orcalog::MasterFileReader *masterFileReader_;

    std::vector<orcalog::ReplayerFactory*>           replayerFactories_;
    std::vector<hydrodll::DynamicallyLoadedLibrary*> libraries_;

    std::vector<orcalog::Replayer*> replayers_;

    // Ensure interfaces of a given type are unique.
    InterfaceTypeCounter interfaceTypeCounter_;
    
};

} // namespace

#endif
