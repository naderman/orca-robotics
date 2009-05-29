/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <sstream>
#include <Ice/Ice.h>
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

void
MainThread::walk()
{
    // we provide no interfaces: so there's no need to activate the component.

    orcacm::RegistryHomeData regData;

    // try getting home data a few times  
    while(!isStopping())
    {
        //cout<<"default locator (refresh) :"<<context_.communicator()->getDefaultLocator()->ice_toString()<<endl;
    
        // 
        // Remote call!
        // this just returns a list of homes, without pinging
        regData = orcacm::getRegistryHomeData( context_,
                            context_.communicator()->getDefaultLocator()->ice_toString() );
    
        if ( regData.isReachable ) {
            break;
        }

        context_.tracer().warning( "Registry unreachable. Will try again in 2 seconds" );
        gbxiceutilacfr::checkedSleep( this, 2000 );
    }

    std::ostringstream os;
    os<<"Retrieved list of "<<regData.homes.size()<<" Home objects";
    context_.tracer().info( os.str() );

    orcacm::ComponentData compData;
    
    for ( unsigned int i=0; i<regData.homes.size(); ++i ) 
    {
        // 
        // When the user hits Ctrl-C, we just want to quit
        //
        if ( isStopping() )
            return;

        // 
        // Remote call!
        // get full component data based on the component header information
        compData = orcacm::getComponentHomeData( context_, regData.homes[i].proxy );
        compData.locatorString = regData.locatorString;
        compData.adminAddress = regData.address;
        
        cout<<orcaice::toString( compData.name )<<endl;

        if ( compData.isReachable )
        {
            // look at each interface
            for ( unsigned int j=0; j<compData.provides.size(); ++j ) {
                cout<<" -o "<<compData.provides[j].name<<endl;
            }
            for ( unsigned int j=0; j<compData.requires.size(); ++j ) {
                cout<<" -c "<<orcaice::toString( compData.requires[j].name )<<endl;
            }
        }
        else
        {
            cout<<" x unreachable"<<endl;
        }
    }

    // normally ctrl-c handler does this, now we have to do this because we are finished
    context_.shutdown();
}

} // namespace
