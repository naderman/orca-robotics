/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2006-2007 Alexei Makarenko, Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <sstream>

#include <IceGrid/Registry.h>
#include <IceGrid/Observer.h>
#include <orcaice/orcaice.h>
#include <orcalog/orcalog.h>

#include "sessioncreationcallback.h"
#include "component.h"

using namespace std;

namespace icegridmon {


//////////////////////////////////////////////////////////////////////

//
// This will get called post-activation.
//
// We need to fuck about like this because we don't have a thread in
// which to perform setup, and we want to make sure we're activated
// before registering observers.
//
class SessionManagerStarter : public orcaice::PostActivationCallback
{
public:
    SessionManagerStarter( orcaicegrid::SessionManager &sessionManager )
        : sessionManager_(sessionManager) {}

    void actionPostActivation()
        { sessionManager_.start(); }

private:

    orcaicegrid::SessionManager &sessionManager_;
};

//////////////////////////////////////////////////////////////////////

Component::Component( const std::string & compName )
    : orcaice::Component( compName ),
      sessionCreationCallback_(0),
      sessionManager_(0)
{
}

Component::~Component()
{
    delete sessionCreationCallback_;
    delete sessionManagerStarter_;
}

void 
Component::start()
{
    sessionCreationCallback_ = new SessionCreationCallback( context() );
    sessionManager_ = new orcaicegrid::SessionManager( *sessionCreationCallback_, context() );
    sessionManagerStarter_ = new SessionManagerStarter( *sessionManager_ );

    // Don't have to keep track of the activator: it'll either:
    // a) activate us, then start the sessionManager, or
    // b) fail to activate us, and self-destruct at the end of the component's lifetime.
    orcaice::Activator *activator = new orcaice::Activator( context(), sessionManagerStarter_ );
    activator->start();
}

void 
Component::stop()
{
    tracer()->debug("stopping component...",2);
    hydroutil::stopAndJoin( sessionManager_ );
}

} // namespace
