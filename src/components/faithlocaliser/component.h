/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCA2_FAITH_LOCALISER_COMPONENT_H
#define ORCA2_FAITH_LOCALISER_COMPONENT_H

#include <orcaice/component.h>
#include <orcaice/ptrbuffer.h>
#include <orcaice/bufferedconsumerI.h>

#include <orca/localise2d.h>
#include <orca/position2d.h>

namespace faithlocaliser
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

    MainLoop *mainLoop_;

    //
    // EXTERNAL PROVIDED INTERFACE
    //
    orca::Localise2dConsumerPrx localise2dPublisher_;

    orcaice::Buffer<orca::Localise2dData> locBuffer_;
    orcaice::Buffer<orca::Localise2dData> historyBuffer_;

    // REQUIRED INTERFACE
    orcaice::Buffer<orca::Position2dData> posPipe_;

};

} // namespace

#endif
