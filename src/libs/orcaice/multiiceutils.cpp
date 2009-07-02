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
#include <Ice/Ice.h>
#include <gbxsickacfr/gbxiceutilacfr/threadutils.h>

#include "multiiceutils.h"
#include "iceutils.h"
// #include "configutils.h"
#include "exceptions.h"

using namespace std;

namespace orcaice {

void createInterfaceWithString( const Context& context,
                Ice::ObjectPtr& object,
                const std::string& name,
                gbxutilacfr::Stoppable* activity, const std::string& subsysName, 
                int retryIntervalSec, int retryNumber )
{
    assert( activity && "Null activity pointer" );

    int count = 0;
    while ( !activity->isStopping() && ( retryNumber<0 || count<retryNumber) )
    {
        try {
            createInterfaceWithString( context, object, name );
            if ( !subsysName.empty() )
                context.status().ok( subsysName );
            break;
        }
        catch ( const std::exception& e ) {
            std::stringstream ss;
            ss << "Failed to create interface with string "<<name<<": " << e.what() << endl
               <<"Will retry in "<<retryIntervalSec<<"s.\n";
            if ( !subsysName.empty() )
                context.status().warning( subsysName, ss.str() );
            else
                context.tracer().warning( subsysName, ss.str() );
        }
        catch ( ... ) {
            std::stringstream ss;
            ss << "Caught something while creating interface with string "<<name<<". " << endl
                <<"Will retry in "<<retryIntervalSec<<"s.\n";
            if ( !subsysName.empty() )
                context.status().warning( subsysName, ss.str() );
            else
                context.tracer().warning( subsysName, ss.str() );
        }
        ++count;
        gbxiceutilacfr::checkedSleep( activity, retryIntervalSec*1000 );
    }
}

void createInterfaceWithTag( const Context& context,
            Ice::ObjectPtr& object,
            const std::string& interfaceTag,
            gbxutilacfr::Stoppable* activity, const std::string& subsysName, 
            int retryIntervalSec, int retryNumber )
{
    assert( activity && "Null activity pointer" );

    int count = 0;
    while ( !activity->isStopping() && ( retryNumber<0 || count<retryNumber) )
    {
        try {
            createInterfaceWithTag( context, object, interfaceTag );
            if ( !subsysName.empty() )
                context.status().ok( subsysName );
            break;
        }
        catch ( const std::exception& e ) {
            std::stringstream ss;
            ss << "Failed to create interface with tag "<<interfaceTag<<" : " << e.what() << endl
               <<"Will retry in "<<retryIntervalSec<<"s.";
            if ( !subsysName.empty() )
                context.status().warning( subsysName, ss.str() );
            else
                context.tracer().warning( subsysName, ss.str() );
        }
        catch ( ... ) {
            std::stringstream ss;
            ss << "Caught something while creating interface with tag "<<interfaceTag<<". "<<endl
                <<"Will retry in "<<retryIntervalSec<<"s.\n";
            if ( !subsysName.empty() )
                context.status().warning( subsysName, ss.str() );
            else
                context.tracer().warning( subsysName, ss.str() );
        }
        ++count;
        gbxiceutilacfr::checkedSleep( activity, retryIntervalSec*1000 );
    }
}

void activate( Context& context, 
            gbxutilacfr::Stoppable* activity, const std::string& subsysName, 
            int retryIntervalSec, int retryNumber )
{
    assert( activity && "Null activity pointer" );

    int count = 0;
    while ( !activity->isStopping() && ( retryNumber<0 || count<retryNumber) )
    {
        try {
            context.activate();
            break;
        }
        catch ( Ice::CommunicatorDestroyedException )
        {
            // This means we're shutting down.
            break;
        }
        catch ( const std::exception& e ) {
            std::stringstream ss;
            ss << "Failed to activate component : "<<e.what()<<endl
               <<"Will retry in "<<retryIntervalSec<<"s.\n";
            context.tracer().warning( subsysName, ss.str() );
        }
        catch ( ... ) {
            std::stringstream ss;
            ss << "Caught something while activating. " << endl
                <<"Will retry in "<<retryIntervalSec<<"s.\n";
            context.tracer().warning( subsysName, ss.str() );
        }
        ++count;
        gbxiceutilacfr::checkedSleep( activity, retryIntervalSec*1000 );
        if ( !subsysName.empty() )
            context.status().heartbeat( subsysName );
    }
}

} // namespace
