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

// Context::Context( const Context& orig ) :
//     tag_(orig.tag_),
//     name_(orig.name_),
//     isApplication_(orig.isApplication_),
//     communicator_(orig.communicator_),
//     adapter_(orig.adapter_),
//     component_(orig.component_),
//     home_(orig.home_),
//     tracer_(orig.tracer_),
//     status_(orig.status_) 
// {};
// 
// void 
// Context::operator=( const Context& orig )
// {
//     tag_ = orig.tag_;
//     name_ = orig.name_;
//     isApplication_ = orig.isApplication_;
//     communicator_ = orig.communicator_;
//     adapter_ = orig.adapter_;
//     component_ = orig.component_;
//     home_ = orig.home_;
//     tracer_ = orig.tracer_;
//     status_ = orig.status_; 
// };

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
