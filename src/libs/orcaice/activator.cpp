/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "activator.h"
#include <iostream>
#include <orcaice/exceptions.h>
#include <IceUtil/Thread.h>

using namespace std;

namespace orcaice {

Activator::Activator( const orcaice::Context &context,
                      PostActivationCallback *postActivationCallback )
    : context_(context),
      postActivationCallback_(postActivationCallback)
{
}

Activator::~Activator()
{
}

void 
Activator::run()
{
    while ( !isStopping() )
    {
        try {
            context_.tracer()->debug( "Activator: Activating..." );
            context_.activate();
            context_.tracer()->debug( "Activator: Activated." );
            break;
        }
        catch ( orcaice::ComponentDeactivatingException &e )
        {
            // We must be going down...
            std::stringstream ss;
            ss << "Activator: Giving up because component is going down: " << e.what();
            context_.tracer()->debug( ss.str() );
            break;
        }            
        catch ( std::exception &e )
        {
            std::stringstream ss;
            ss << "Activator: Caught exception: " << e.what();
            context_.tracer()->warning( ss.str() );
        }
        catch ( ... )
        {
            context_.tracer()->warning( "Activator: caught unknown exception." );
        }
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
    }

    if ( postActivationCallback_ != NULL )
    {
        context_.tracer()->debug( "Activator: calling postActivationCallback_" );
        postActivationCallback_->actionPostActivation();
    }

    context_.tracer()->debug( "Activator: dropping out of run()", 3 );
}

}

