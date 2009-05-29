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
            if ( !subsysName.empty() )
                context.status().ok( subsysName );
            break;
        }
        catch ( const std::exception& e ) {
            std::stringstream ss;

            ss << "Failed to get interface ID with string="<<proxyString<<" : "<< e.what()<<endl
               << "Check Registry. Will retry in "<<retryIntervalSec<<"s.";
            if ( !subsysName.empty() )
                context.status().warning( subsysName, ss.str() );
            else
                context.tracer().warning( ss.str() );
        }
        ++count;
        gbxiceutilacfr::checkedSleep( activity, retryIntervalSec*1000 );
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
            if ( !subsysName.empty() )
                context.status().ok( subsysName );
            break;
        }
        catch ( const std::exception& e ) {
            std::stringstream ss;
            ss << "Failed to get interface ID with tag="<<interfaceTag<<" : "<< e.what()<<endl
               << "Check Registry. Will retry in "<<retryIntervalSec<<"s.";
            if ( !subsysName.empty() )
                context.status().warning( subsysName, ss.str() );
            else
                context.tracer().warning( ss.str() );
        }
        ++count;
        gbxiceutilacfr::checkedSleep( activity, retryIntervalSec*1000 );
    }
    return ifaceId;
}

} // namespace
