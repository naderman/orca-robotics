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

#include "multiconnectutils.h"
#include "configutils.h"
#include "exceptions.h"

using namespace std;

namespace orcaice {

std::string 
getInterfaceIdWithString( const Context& context, const std::string& proxyString,
                            gbxiceutilacfr::Thread*  thread, const std::string& subsysName, 
                            int retryInterval, int retryNumber )
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
            context.tracer().warning( ss.str() );
        }
        ++count;
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(retryInterval));
        if ( !subsysName.empty() ) {
            context.status().heartbeat( subsysName );
        }
    }
    return ifaceId;
}

std::string 
getInterfaceIdWithTag( const Context& context, const std::string& interfaceTag,
                            gbxiceutilacfr::Thread*  thread, const std::string& subsysName, 
                            int retryInterval, int retryNumber )
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
            context.tracer().warning( ss.str() );
        }
        ++count;
        IceUtil::ThreadControl::sleep(IceUtil::Time::seconds(retryInterval));
        if ( !subsysName.empty() ) {
            context.status().heartbeat( subsysName );
        }
    }
    return ifaceId;
}

} // namespace
