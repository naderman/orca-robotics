/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_NOISY_ROBOT_COMPONENT_H
#define ORCA2_NOISY_ROBOT_COMPONENT_H

#include <orcaice/component.h>
#include "inoisemodel.h"

namespace noisyrobot2d
{

class HwHandler;
class NetHandler;

class Component : public orcaice::Component
{
public:

    Component();
    virtual ~Component();

    // component interface
    virtual void  start();
    virtual void stop();

private:

    INoiseModel *noiseModel_;

};

} // namespace

#endif
