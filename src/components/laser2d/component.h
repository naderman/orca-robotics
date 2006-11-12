/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_LASER2D_COMPONENT_H
#define ORCA2_LASER2D_COMPONENT_H

#include <orcaice/component.h>
#include <orcaice/ptrbuffer.h>

#include <orca/laserscanner2d.h>

namespace sicklaser {

class MainLoop;
class Driver;

class Component : public orcaice::Component
{
public:

    Component();
    virtual ~Component();

    virtual void start();
    virtual void stop();

private:

    MainLoop *mainLoop_;

    // need to store it as a member variable, even though the adapter will keep it alive
    // when the adapter dies on ctrl-c, this object would be destroyed trying to use the smart
    // pointer results in seg fault.
    Ice::ObjectPtr laserObj_;
    
    //
    // HARDWARE INTERFACES
    //
    Driver *hwDriver_;
};

}

#endif
