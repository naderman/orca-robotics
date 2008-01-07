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

#include <orcaice/component.h>
#include <orcaifaceimpl/pixmapImpl.h>

namespace pixmaploader {

class Component : public orcaice::Component
{
public:
    Component();

    // from orcaice::Component
    virtual void start();
    virtual void stop();

private:

    // component owns the interface object, the thread
    // will simply initialize it and exit.
    orcaifaceimpl::PixMapImplPtr pixMapImpl_;
    
    hydroiceutil::ThreadPtr thread_;
};

}

#endif
