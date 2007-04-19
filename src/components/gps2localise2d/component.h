/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA2_GPS2LOCALISE2D_COMPONENT_H
#define ORCA2_GPS2LOCALISE2D_COMPONENT_H

#include <orcaice/component.h>

// interface definitions
#include <orcaifaceimpl/localise2diface.h>

namespace gps2localise2d
{

class Component : public orcaice::Component
{
public:

    Component();
    virtual ~Component();

    // component interface
    virtual void start();
    virtual void stop();

private:

    // External interface
    orcaifaceimpl::Localise2dIfacePtr localiseInterface_;

    orcaice::ThreadPtr mainLoop_;
};

} // namespace

#endif
