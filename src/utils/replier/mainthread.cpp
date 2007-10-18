/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <orcaice/orcaice.h>

#include "handler.h"
#include "replierI.h"

using namespace std;

namespace replier {

Handler::Handler( const orcaice::Context & context ) : 
    context_(context)
{
}

Handler::~Handler()
{
}

void
Handler::run()
{
    // we are in a different thread now, catch all stray exceptions
    try
    {
        // The only provided interfaces are the 2 standard ones: Home and Status.
        // Just keep trying till this works...
        while ( !isStopping() )
        {
            try {
                context_.activate();
                break;
            }
            catch ( orcaice::NetworkException & e )
            {
                cout<<e.what()<<endl;
            }
            catch ( Ice::Exception & e )
            {
                cout<<e<<endl;
            }
            catch ( ... )
            {
                cout << "Caught some other exception while activating." << endl;
            }
    
            context_.tracer()->debug( "Failed to activate. Will try again...", 3 );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
        }

        // create a provided interface
        // create servant for direct connections and tell adapter about it
        // don't need to store it as a member variable, adapter will keep it alive
        Ice::ObjectPtr replierObj = new ReplierI;
        // two possible exceptions will kill it here, that's what we want
        orcaice::createInterfaceWithTag( context_, replierObj, "Replier" );

        //
        // Main loop
        //   
        while(!isStopping())
        {
//             cout<<"running main loop"<<endl;
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
        }

    //
    // unexpected exceptions
    //
    } // try
    catch ( const orca::OrcaException & e )
    {
        context_.tracer()->print( e.what );
        context_.tracer()->error( "unexpected (remote?) orca exception.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const Ice::Exception & e )
    {
        cout<<e<<endl;
        context_.tracer()->error( "unexpected Ice exception.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const std::exception & e )
    {
        // once caught this beast in here, don't know who threw it 'St9bad_alloc'
        cout<<e.what()<<endl;
        context_.tracer()->error( "unexpected std exception (possibly orcaice).");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( ... )
    {
        context_.tracer()->error( "unexpected exception from somewhere.");
        if ( context_.isApplication() ) {
            context_.tracer()->info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }

    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
    context_.tracer()->debug( "handler stopped", 2 );
}

} // namespace
