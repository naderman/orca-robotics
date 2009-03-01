/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
 
#ifndef COMPONENT_H
#define COMPONENT_H

#include <string>
#include <orcaice/component.h>
#include <hydrodll/dynamicload.h>
#include <hydroqgui/hydroqgui.h>

namespace orcaview3d {
    
class Component : public orcaice::Component
{
public:
    Component();
    ~Component();

    // component interface
    virtual void start();
    virtual void stop();

private:

    std::vector<hydroqgui::IGuiElementFactory*>      factories_;
    std::vector<hydrodll::DynamicallyLoadedLibrary*> libraries_;
        
    // loads all factory libs and returns a listing of supported interfaces
    std::vector<std::string> loadPluginLibraries( const std::string &factoryLibNames );  
};

}

#endif

