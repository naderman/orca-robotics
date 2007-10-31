/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp,
 *               George Mathews, Stef Williams.
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_LASER_FEATURE_EXTRACTOR_COMPONENT_H
#define ORCA2_LASER_FEATURE_EXTRACTOR_COMPONENT_H

#include <orcaice/component.h>

// interface definitions
#include <orcaifaceimpl/polarfeature2dImpl.h>

namespace laserfeatures
{

class MainLoop;

class Component : public orcaice::Component
{
public:

    Component();
    virtual ~Component();

    // component interface
    virtual void start();
    virtual void stop();

private:

    // External interface: polar features
    orcaifaceimpl::PolarFeature2dImplPtr featureInterface_;

    MainLoop *mainLoop_;
};

} // namespace

#endif
