/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#include "pinger.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <sstream>

using namespace std;

namespace hydroping {

Pinger::Pinger( double maxTimeoutSec )
{
    pingObj_ = ping_construct();
    int ret = ping_setopt( pingObj_, PING_OPT_TIMEOUT, &maxTimeoutSec );
    if ( ret < 0 )
    {
        ping_destroy( pingObj_ );
        stringstream ss;
        ss << "Failed to set timeout to " << maxTimeoutSec << "s";
        throw PingException( ERROR_INFO, std::string( ss.str() ) );
    }
}

Pinger::~Pinger()
{
    ping_destroy( pingObj_ );
}

double 
Pinger::ping( const std::string &host )
{
    int ret;

    ret = ping_host_add( pingObj_, host.c_str() );
    if ( ret != 0 )
        throw PingException( ERROR_INFO, "Error when pinging "+host+": "+ping_get_error( pingObj_ ) );

    ret = ping_send( pingObj_ );
    if ( ret < 0 )
    {
        ping_host_remove( pingObj_, host.c_str() );
        throw PingException( ERROR_INFO, "Error when pinging "+host+": "+ping_get_error( pingObj_ ) );
    }

    pingobj_iter_t *pingObjIter = ping_iterator_get( pingObj_ );
    if ( pingObjIter == NULL )
    {
        ping_host_remove( pingObj_, host.c_str() );
        throw PingException( ERROR_INFO, "Error when pinging "+host+": "+ping_get_error( pingObj_ ) );
    }

    double latencyMs;
    size_t latencyLen = sizeof(latencyMs);
    ret = ping_iterator_get_info( pingObjIter,
                                  PING_INFO_LATENCY,
                                  (void*)(&latencyMs),
                                  &latencyLen );
    if ( ret != 0 )
    {
        ping_host_remove( pingObj_, host.c_str() );
        throw PingException( ERROR_INFO, "Error when pinging "+host+": "+strerror( ret ) );    
    }

    ret = ping_host_remove( pingObj_, host.c_str() );
    if ( ret != 0 )
        throw PingException( ERROR_INFO, "Error when pinging "+host+": "+ping_get_error( pingObj_ ) );

    return latencyMs;
}


}
