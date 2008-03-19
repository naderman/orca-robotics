/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef ORCAICE_ACTIVATOR_H
#define ORCAICE_ACTIVATOR_H

#include <gbxsickacfr/gbxiceutilacfr/safethread.h>
#include <orcaice/context.h>

namespace orcaice {

//!
//! @brief A hook to allow something to be done post-activation
//!
//! @author Alex Brooks
//!
class PostActivationCallback {
public:
    virtual ~PostActivationCallback() {}

    virtual void actionPostActivation()=0;
};

//!
//! @brief A 'fire-and-forget' component activator.
//!
//! This thing will keep trying to activate the component until either:
//! (a) it succeeds (in which case it self-destructs), or
//! (b) the component shuts down.
//!
//! In place of 'activate()' (which may fail), use the activator like
//! so in the component's start() function:
//! 
//! @verbatim
//! orcaice::Activator *activator = new orcaice::Activator( context() );
//! activator->start();
//!
//!  ... (now forget about it) ...
//!
//! @endverbatim
//!
//! @author Alex Brooks
//!
class Activator : public gbxsickacfr::gbxiceutilacfr::SafeThread
{

public: 

    Activator( const orcaice::Context &context,
               PostActivationCallback *postActivationCallback=NULL );
    ~Activator();

    virtual void walk();

private: 

    orcaice::Context  context_;

    PostActivationCallback *postActivationCallback_;

};

}

#endif
