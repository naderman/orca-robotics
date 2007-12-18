/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef COMPONENT_H
#define COMPONENT_H

#include <orcaice/component.h>
#include <orcaifaceimpl/featuremap2dImpl.h>

namespace featuremaploader {

//
// Deriving from orcaice::Component means we have to implement start() and stop()
//
class Component : public orcaice::Component
{
public:

    Component();

    virtual void start();
    virtual void stop() {};

private:

    // Returns zero on success
    int loadMap( const std::string &fileName, orca::FeatureMap2dData &theMap );

    //
    // EXTERNAL INTERFACE: featuremap2d
    //
    orcaifaceimpl::FeatureMap2dImplPtr featureMap2dImpl_;
};

}

#endif
