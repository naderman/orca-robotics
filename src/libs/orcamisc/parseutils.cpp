/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#include "parseutils.h"

namespace orcamisc {

std::string 
connectionToLocalAddress( const std::string & s )
{
    int equal = s.find( '=', 0 );
    int remote = s.find( "remote", equal );
    
    return s.substr( equal+2, remote-2 );
}

std::string 
connectionToRemoteAddress( const std::string & s )
{
    int remote = s.find( "remote", 0 );
    int equal = s.find( '=', remote );
    
    return s.substr( equal+2, s.npos );
}

std::string 
stringToIceGridInstanceName( const std::string & s )
{
    int slash = s.find( "/", 0 );
    
    return s.substr( 0, slash );
}

}
