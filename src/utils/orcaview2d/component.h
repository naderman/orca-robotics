/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCAVIEW_COMPONENT_H
#define ORCAVIEW_COMPONENT_H

#include <string>
#include <orcaice/component.h>
#include <hydrodll/dynamicload.h>
#include <orcaqgui/guielementfactory.h>

using namespace std;

namespace orcaview2d {
    
class Component : public orcaice::Component
{
public:
    Component( std::string compName );
    ~Component();

    // component interface
    virtual void start();
    virtual void stop();

private:

    std::vector<orcaqgui::GuiElementFactory*>               factories_;
    std::vector<hydrodll::DynamicallyLoadedLibrary*> libraries_;
        
    // loads all factory libs and returns a listing of supported interfaces
    std::vector<std::string> loadPluginLibraries( const std::string &factoryLibNames );  
};

}

#endif

