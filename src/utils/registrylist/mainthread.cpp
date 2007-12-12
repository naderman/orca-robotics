/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <sstream>
#include <orcaice/orcaice.h>
#include <orcacm/orcacm.h>

#include "mainthread.h"

using namespace std;

namespace registrylist {

MainThread::MainThread( const orcaice::Context & context ) :
    SafeThread(context.tracer()),
    context_(context)
{
}

MainThread::~MainThread()
{
}

void
MainThread::walk()
{
    // we are in a different thread now, catch all stray exceptions
    try
    {
        // we provide no interfaces: so there's no need to activate the component.

        orcacm::RegistryHomeData regData;

        //
        // Main loop
        //   
        while(!isStopping())
        {
            //cout<<"default locator (refresh) :"<<context_.communicator()->getDefaultLocator()->ice_toString()<<endl;
        
            // don't try to ping adapters here, we'll have to do it anyway below, when we get their full information
            bool pingAdapters = false;
            regData = orcacm::getRegistryHomeData( 
                                context_,
                                context_.communicator()->getDefaultLocator()->ice_toString(), 
                                pingAdapters );
        
            if ( regData.isReachable ) {
                break;
            }

            context_.tracer().warning( "Registry unreachable. Will try again in 2 seconds" );
            IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(2));
        }

        std::ostringstream os;
        os<<"Retrieved list of "<<regData.homes.size()<<" Home objects";
        context_.tracer().info( os.str() );
    
        orcacm::ComponentData compData;
        
        for ( unsigned int i=0; i<regData.homes.size(); ++i ) {
            // get full component data based on the component header information
//             compData = orcacm::getComponentData( context_, regData.adapters[i].name );
            compData = orcacm::getComponentHomeData( context_, regData.homes[i].proxy );
            compData.locatorString = regData.locatorString;
            compData.adminAddress = regData.address;
            
            cout<<orcaice::toString( compData.name )<<endl;

            IceUtil::Time t = IceUtil::Time::seconds(compData.timeUp);
            cout<<"    time up: "<<t.toDuration()<<endl;

            // now look at each interface
            for ( unsigned int j=0; j<compData.provides.size(); ++j ) {
                cout<<" -o "<<compData.provides[j].name<<endl;
            }
            for ( unsigned int j=0; j<compData.requires.size(); ++j ) {
                cout<<" -c "<<orcaice::toString( compData.requires[j].name )<<endl;
            }
        }

        // normally ctrl-c handler does this, now we have to do this because we are finished
        context_.communicator()->shutdown();

    //
    // unexpected exceptions
    //
    } // try
    catch ( const orca::OrcaException & e )
    {
        stringstream ss;
        ss << "unexpected (remote?) orca exception: " << e << ": " << e.what;
        context_.tracer().error( ss.str() );
        if ( context_.isApplication() ) {
            context_.tracer().info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const hydroutil::Exception & e )
    {
        stringstream ss;
        ss << "unexpected (local?) orcaice exception: " << e.what();
        context_.tracer().error( ss.str() );
        if ( context_.isApplication() ) {
            context_.tracer().info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const Ice::Exception & e )
    {
        stringstream ss;
        ss << "unexpected Ice exception: " << e;
        context_.tracer().error( ss.str() );
        if ( context_.isApplication() ) {
            context_.tracer().info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( const std::exception & e )
    {
        // once caught this beast in here, don't know who threw it 'St9bad_alloc'
        stringstream ss;
        ss << "unexpected std exception: " << e.what();
        context_.tracer().error( ss.str() );
        if ( context_.isApplication() ) {
            context_.tracer().info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }
    catch ( ... )
    {
        context_.tracer().error( "unexpected exception from somewhere.");
        if ( context_.isApplication() ) {
            context_.tracer().info( "this is an stand-alone component. Quitting...");
            context_.communicator()->destroy();
        }
    }

    // wait for the component to realize that we are quitting and tell us to stop.
    waitForStop();
    context_.tracer().debug( "handler stopped", 5 );
}

} // namespace
