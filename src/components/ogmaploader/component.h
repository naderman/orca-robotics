/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef OGMAPLOADER_COMPONENT_H
#define OGMAPLOADER_COMPONENT_H

#include <orcaice/component.h>
#include <orca/ogmap.h>
#include <orcaifaceimpl/ogmapImpl.h>

namespace ogmaploader {

//
// Deriving from orcaice::Component means we have to implement start() and stop()
//
class Component : public orcaice::Component
{
public:

    Component();
    virtual ~Component();

    virtual void start();
    virtual void stop();

private:

    //
    // EXTERNAL INTERFACE
    //
    orcaifaceimpl::OgMapImplPtr ogMapInterface_;
};

}

#endif
