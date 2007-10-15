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
                            orcaiceutil::Thread*  thread, int retryInterval, int retryNumber )
{
    int count = 0;
    while ( !thread->isStopping() && ( retryNumber<0 || count<retryNumber) )
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
        ++count;
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(retryInterval));
    }
}

void 
createInterfaceWithTag( const Context       & context,
                        Ice::ObjectPtr      & object,
                        const std::string   & interfaceTag,
                        orcaiceutil::Thread*  thread, int retryInterval, int retryNumber )
{
    int count = 0;
    while ( !thread->isStopping() && ( retryNumber<0 || count<retryNumber) )
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
        ++count;
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(retryInterval));
    }
}

void
activate( Context& context, orcaiceutil::Thread* thread, int retryInterval, int retryNumber )
{
    int count = 0;
    while ( !thread->isStopping() && ( retryNumber<0 || count<retryNumber) )
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
        ++count;
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(retryInterval));
    }
}

std::string 
getInterfaceIdWithString( const Context& context, const std::string& proxyString,
                       orcaiceutil::Thread*  thread, int retryInterval, int retryNumber )
{
    std::string ifaceId;
    int count = 0;
    while ( !thread->isStopping() && ( retryNumber<0 || count<retryNumber) )
    {
        try {
            ifaceId = getInterfaceIdWithString( context, proxyString );
            break;
        }
        catch ( const std::exception& e ) {
            std::stringstream ss;
            ss << "Failed to get interface ID with string="<<proxyString<<". Check Registry. "
                <<"Will retry in "<<retryInterval<<"s.\n"
                << e.what();
            context.tracer()->warning( ss.str() );
        }
        ++count;
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(retryInterval));
    }
    return ifaceId;
}

std::string 
getInterfaceIdWithTag( const Context& context, const std::string& interfaceTag,
                       orcaiceutil::Thread*  thread, int retryInterval, int retryNumber )
{
    std::string ifaceId;
    int count = 0;
    while ( !thread->isStopping() && ( retryNumber<0 || count<retryNumber) )
    {
        try {
            ifaceId = getInterfaceIdWithTag( context, interfaceTag );
            break;
        }
        catch ( const std::exception& e ) {
            std::stringstream ss;
            ss << "Failed to get interface ID with tag="<<interfaceTag<<". Check Registry. "
                <<"Will retry in "<<retryInterval<<"s.\n"
                << e.what();
            context.tracer()->warning( ss.str() );
        }
        ++count;
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(retryInterval));
    }
    return ifaceId;
}

} // namespace
