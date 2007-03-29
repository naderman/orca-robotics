/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp,
 *               George Mathews, Stef Williams.
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_LASER_FEATURE_EXTRACTOR_COMPONENT_H
#define ORCA2_LASER_FEATURE_EXTRACTOR_COMPONENT_H

#include <orcaice/component.h>
#include <orcaice/ptrbuffer.h>

// interface definitions
#include <orca/laserscanner2d.h>
#include <orca/polarfeature2d.h>
#include <orcaifaceimpl/polarfeature2diface.h>

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
    orcaifaceimpl::PolarFeature2dIfacePtr featureInterface_;

    MainLoop *mainLoop_;
};

} // namespace

#endif
