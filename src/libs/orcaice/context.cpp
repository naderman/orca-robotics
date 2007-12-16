/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "context.h"
#include "component.h"

namespace orcaice {

Context::Context() :
    component_(0),
    home_(0),
    tracer_(0),
    status_(0)
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

hydroutil::Context 
Context::toHydroContext() const
{
    return toHydroContext( tag()+".Config." );
}

hydroutil::Context 
Context::toHydroContext( const std::string &prefixToStrip ) const
{
    return hydroutil::Context( hydroutil::Properties( properties()->getPropertiesForPrefix(prefixToStrip), prefixToStrip ), 
                               tracer(), 
                               status() );
}

} // namespace
