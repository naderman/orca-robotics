/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "context.h"
#include "component.h"
#include <hydroutil/exceptions.h>

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
    if ( !component_ )
        throw hydroutil::Exception( ERROR_INFO, "Trying to activate component before context initializition." );

    component_->activate();
}

void
Context::shutdown()
{
    if ( !communicator_ )
        throw hydroutil::Exception( ERROR_INFO, "Trying to shutdown component before context initializition." );

    if ( isApplication() ) {
        tracer().info( "Triggering component shutdown ...");
        communicator()->destroy();
    }
    else {
        tracer().info( "NOT triggering component shutdown because running as a service ...");
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
    // transfer Orca properties into Hydro configs
    hydroutil::Properties::Config config;
    if ( properties()->getPropertyAsIntWithDefault( "Orca.Warn.DefaultProperty", 1 ) )
        config.warnDefaultProperty = true;

    return hydroutil::Context( hydroutil::Properties( properties()->getPropertiesForPrefix(prefixToStrip), prefixToStrip, config ), 
                               tracer(), 
                               status() );
}

} // namespace
