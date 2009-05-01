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
                            gbxutilacfr::Stoppable* activity, const std::string& subsysName, 
                            int retryIntervalSec, int retryNumber )
{
    assert( activity && "Null activity pointer" );

    std::string ifaceId;
    int count = 0;
    while ( !activity->isStopping() && ( retryNumber<0 || count<retryNumber) )
    {
        try {
            ifaceId = getInterfaceIdWithString( context, proxyString );
            break;
        }
        catch ( const std::exception& e ) {
            std::stringstream ss;
            ss << "Failed to get interface ID with string="<<proxyString<<". Check Registry. "
                <<"Will retry in "<<retryIntervalSec<<"s.\n"
                << e.what();
            context.tracer().warning( ss.str() );
        }
        ++count;
        gbxiceutilacfr::checkedSleep( activity, retryIntervalSec*1000 );
        if ( !subsysName.empty() ) {
            context.status().heartbeat( subsysName );
        }
    }
    return ifaceId;
}

std::string 
getInterfaceIdWithTag( const Context& context, const std::string& interfaceTag,
                            gbxutilacfr::Stoppable* activity, const std::string& subsysName, 
                            int retryIntervalSec, int retryNumber )
{
    assert( activity && "Null activity pointer" );

    std::string ifaceId;
    int count = 0;
    while ( !activity->isStopping() && ( retryNumber<0 || count<retryNumber) )
    {
        try {
            ifaceId = getInterfaceIdWithTag( context, interfaceTag );
            break;
        }
        catch ( const std::exception& e ) {
            std::stringstream ss;
            ss << "Failed to get interface ID with tag="<<interfaceTag<<". Check Registry. "
                <<"Will retry in "<<retryIntervalSec<<"s.\n"
                << e.what();
            context.tracer().warning( ss.str() );
        }
        ++count;
        gbxiceutilacfr::checkedSleep( activity, retryIntervalSec*1000 );
        if ( !subsysName.empty() ) {
            context.status().heartbeat( subsysName );
        }
    }
    return ifaceId;
}

} // namespace
