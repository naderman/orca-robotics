/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_TELEOP_COMPONENT_H
#define ORCA2_TELEOP_COMPONENT_H

#include <orcaice/component.h>

namespace teleop
{

class NetworkHandler;
class TermDisplayHandler;
class InputHandler;

class Component : public orcaice::Component
{
public:

    Component();
    virtual ~Component();

    // component interface
    virtual void start();
    virtual void stop();

private:
    NetworkHandler* networkHandler_;
    TermDisplayHandler* displayHandler_;
    InputHandler* inputHandler_;
};

} // namespace

#endif
