/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef PIXMAPLOADER_COMPONENT_H
#define PIXMAPLOADER_COMPONENT_H

#include <orcaice/component.h>
#include <orcaifaceimpl/pixmapImpl.h>

namespace pixmaploader {

class Component : public orcaice::Component
{
public:

    Component();

    virtual void start();
    virtual void stop() {};

private:

    orcaifaceimpl::PixMapImplPtr pixMapInterface_;
    
    void loadMapFromFile(orca::PixMapData &theMap);
};

}

#endif
