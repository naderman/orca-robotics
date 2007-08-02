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

#include "multiconnectutils.h"
#include "configutils.h"
#include "exceptions.h"

using namespace std;

namespace orcaice {

void 
createInterfaceWithString( const Context       & context,
                            Ice::ObjectPtr      & object,
                            const std::string   & name,
                            orcaice::Thread*  thread, int retryInterval )
{
    while ( thread->isActive() )
    {
        try {
            createInterfaceWithString( context, object, name );
            break;
        }     
        catch ( const std::exception& e ) {
            std::stringstream ss;
            ss << "Failed to create interface with string "<<name<<". Check Registry. "
                <<"Will retry in "<<retryInterval<<"s.\n"
                << e.what();
            context.tracer()->warning( ss.str() );
        }
        catch ( ... ) {
            std::stringstream ss;
            ss << "Caught something while creating interface with string "<<name<<". "
                <<"Will retry in "<<retryInterval<<"s.\n";
            context.tracer()->warning( ss.str() );
        }
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(retryInterval));
    }
}

void 
createInterfaceWithTag( const Context       & context,
                        Ice::ObjectPtr      & object,
                        const std::string   & interfaceTag,
                        orcaice::Thread*  thread, int retryInterval )
{
    while ( thread->isActive() )
    {
        try {
            createInterfaceWithTag( context, object, interfaceTag );
            break;
        }     
        catch ( const std::exception& e ) {
            std::stringstream ss;
            ss << "Failed to create interface with tag "<<interfaceTag<<". Check Registry. "
                <<"Will retry in "<<retryInterval<<"s.\n"
                << e.what();
            context.tracer()->warning( ss.str() );
        }
        catch ( ... ) {
            std::stringstream ss;
            ss << "Caught something while creating interface with tag "<<interfaceTag<<". "
                <<"Will retry in "<<retryInterval<<"s.\n";
            context.tracer()->warning( ss.str() );
        }
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(retryInterval));
    }
}

void
activate( Context& context, orcaice::Thread* thread, int retryInterval )
{
    while ( thread->isActive() )
    {
        try {
            context.activate();
            break;
        }     
        // alexm: in all of these exception handlers we would like to just catch
        // std::exception but Ice 3.2 does not overload e.what() so we have to do it
        // separately.
        catch ( const Ice::Exception& e ) {
            std::stringstream ss;
            ss << "Failed to activate component. Check Registry and IceStorm. "
                <<"Will retry in "<<retryInterval<<"s.\n"
                <<e;
            context.tracer()->warning( ss.str() );
        }
        catch ( const std::exception& e ) {
            std::stringstream ss;
            ss << "Failed to activate component. Check Registry and IceStorm. "
                <<"Will retry in "<<retryInterval<<"s.\n"
                <<e.what();
            context.tracer()->warning( ss.str() );
        }
        catch ( ... ) {
            std::stringstream ss;
            ss << "Caught something while activating. "
                <<"Will retry in "<<retryInterval<<"s.\n";
            context.tracer()->warning( ss.str() );
        }
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(retryInterval));
    }
}

} // namespace