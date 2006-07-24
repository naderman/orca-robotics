/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
 
#ifndef ORCA2_STAGE_LOCALISER_COMPONENT_H
#define ORCA2_STAGE_LOCALISER_COMPONENT_H

#include <orcaice/component.h>
#include <orcaice/ptrbuffer.h>

#include <orca/localise2d.h>

namespace simlocaliser
{

class MainLoop;

// Deriving from orcaice::Component means we have to implement start() and stop()
class Component : public orcaice::Component
{
public:

    Component();
    virtual ~Component();

    virtual void start();
    virtual void stop();

private:

    //
    // EXTERNAL PROVIDED INTERFACE
    //
    orcaice::PtrBuffer<orca::Localise2dDataPtr> locBuffer_;

    MainLoop *mainLoop_;
};

} // namespace

#endif
