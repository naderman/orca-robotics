/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA2_STAGE_LOCALISER_COMPONENT_H
#define ORCA2_STAGE_LOCALISER_COMPONENT_H

#include <orcaice/component.h>

namespace simlocaliser
{

class MainLoop;

class Component : public orcaice::Component
{
public:

    Component();
    virtual ~Component();

    // from orcaice::Component
    virtual void start();
    virtual void stop();

private:
    MainLoop* mainLoop_;
};

} // namespace

#endif
