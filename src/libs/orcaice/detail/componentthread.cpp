/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "componentthread.h"
#include <orcaice/exceptions.h>
#include <orcaice/catchutils.h>
#include <orcaice/icegridutils.h>
#include "privateutils.h"
#include <iostream>
#include <IceGrid/Registry.h>  // used to register Home interface as a well-known object

using namespace std;

namespace orcaice {
namespace detail {

ComponentThread::ComponentThread( const orcaice::Context& context ) :
    SafeThread(context.tracer()),
    context_(context)
{
}

void
ComponentThread::walk()
{    
    context_.status().infrastructureInitialising();

    Ice::PropertiesPtr props = context_.properties();
    bool hasStatusInterface = props->getPropertyAsInt( "Orca.Component.EnableStatus" );
    bool hasHomeInterface = props->getPropertyAsInt( "Orca.Component.EnableHome" );
    
//     cout<<"DEBUG: hasStatusInterface="<<hasStatusInterface<<" hasHomeInterface="<<hasHomeInterface<<endl;

    bool registeredHome = false;

    const int sleepIntervalMs = 1000;

    context_.status().infrastructureWorking();

    try {
        while ( !isStopping() )
        {
            bool needToRegisterHome = hasHomeInterface && !registeredHome;

            if ( !needToRegisterHome && !hasStatusInterface )
            {
                context_.tracer().info( "ComponentThread: Nothing left to do, quitting" );
                return;
            }

            if ( !registeredHome && hasHomeInterface )
            {
                registeredHome = tryRegisterHome();
            }

            if ( hasStatusInterface )
            {
                context_.status().process();
            }

            IceUtil::ThreadControl::sleep(IceUtil::Time::milliSeconds(sleepIntervalMs));
        }
    }
    catch ( ... )
    {
        orcaice::catchExceptions( context_.tracer(), "running component utility thread" );
    }

    context_.status().infrastructureFinalising();
}

//
// PROVIDED INTERFACE: Home
// Make Home a well-known object, by adding it to the registry
//
bool
ComponentThread::tryRegisterHome()
{
    try {
        detail::registerHomeInterface( context_ );
    }
    catch ( Ice::Exception& e ) 
    {
        bool requireRegistry = context_.properties()->getPropertyAsInt( "Orca.Component.RequireRegistry" );
        if ( requireRegistry ) {
            std::stringstream ss;
            ss << "Failed to register Home interface: "<<e<<".  Check IceGrid Registry.  You may allow things to continue without registration by setting Orca.Component.RequireRegistry=0.";
            context_.tracer().error( ss.str() );
            return false;
        }
        else {
            std::stringstream ss;
            ss << "Failed to register Home interface: "<<e<<".";
            context_.tracer().warning( ss.str() );
            context_.tracer().info( "You may enforce registration by setting Orca.Component.RequireRegistry=1." );
            // TODO: returning TRUE is misleading! we haven't registered.
            return true;
        }
    }
    return true;
}

}
}
