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
            break;
        }
        catch ( const std::exception& e ) {
            std::stringstream ss;
            ss << "Failed to create interface with string "<<name<<":\n"
               << e.what() << endl
               <<"Will retry in "<<retryIntervalSec<<"s.\n";
            context.tracer().warning( ss.str() );
        }
        catch ( ... ) {
            std::stringstream ss;
            ss << "Caught something while creating interface with string "<<name<<". "
                <<"Will retry in "<<retryIntervalSec<<"s.\n";
            context.tracer().warning( ss.str() );
        }
        ++count;
        gbxiceutilacfr::checkedSleep( activity, retryIntervalSec*1000 );
        if ( !subsysName.empty() ) {
            context.status().heartbeat( subsysName );
        }
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
            break;
        }
        catch ( const std::exception& e ) {
            std::stringstream ss;
            ss << "Failed to create interface with tag "<<interfaceTag<<":\n"
               << e.what() << endl
               <<"Will retry in "<<retryIntervalSec<<"s.";
            context.tracer().warning( ss.str() );
        }
        catch ( ... ) {
            std::stringstream ss;
            ss << "Caught something while creating interface with tag "<<interfaceTag<<". "
                <<"Will retry in "<<retryIntervalSec<<"s.\n";
            context.tracer().warning( ss.str() );
        }
        ++count;
        gbxiceutilacfr::checkedSleep( activity, retryIntervalSec*1000 );
        if ( !subsysName.empty() ) {
            context.status().heartbeat( subsysName );
        }
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
            ss << "Failed to activate component:\n"
               <<e.what()<<endl
               <<"Will retry in "<<retryIntervalSec<<"s.\n";
            context.tracer().warning( ss.str() );
        }
        catch ( ... ) {
            std::stringstream ss;
            ss << "Caught something while activating. "
                <<"Will retry in "<<retryIntervalSec<<"s.\n";
            context.tracer().warning( ss.str() );
        }
        ++count;
        gbxiceutilacfr::checkedSleep( activity, retryIntervalSec*1000 );
        if ( !subsysName.empty() ) {
            context.status().heartbeat( subsysName );
        }
    }
}

} // namespace
