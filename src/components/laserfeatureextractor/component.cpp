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

#include "component.h"
#include "mainthread.h"

using namespace std;
using namespace laserfeatures;

Component::Component()
    : orcaice::Component( "LaserFeatureExtractor" )
{
}

void 
Component::start()
{
    context().tracer().info( "Starting component...",2 );

    mainThread_ = new MainThread( context() );
    mainThread_->start();
}

void 
Component::stop()
{
    context().tracer().debug("Stopping component", 2 );
    hydroutil::stopAndJoin( mainThread_ );
}
