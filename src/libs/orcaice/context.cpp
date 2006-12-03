/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "context.h"
#include "component.h"

namespace orcaice {

Context::Context()
    : component_(0),
      tracer_(0)
{
}

void 
Context::init( const orca::FQComponentName &name,
               const bool                   isApp,
               const Ice::ObjectAdapterPtr &adapter,
               orcaice::Component          *comp )
{
    name_ = name;
    isApplication_ = isApp;
    adapter_ = adapter;
    component_ = comp;

    communicator_ = adapter_->getCommunicator();
};

void
Context::activate()
{
    if ( component_ ) {
        component_->activate();
    }
}

} // namespace
