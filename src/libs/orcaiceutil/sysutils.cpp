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
#include <stdlib.h> // for getenv()

#include "sysutils.h"
#include "exceptions.h"
#ifndef WIN32
#include <unistd.h>
#endif 

using namespace std;

namespace orcaiceutil
{

std::string 
getHostname()
{
#ifndef WIN32 
    char hostname[256];
    int ret = gethostname(hostname,256);
    if ( ret==0 ) {
        return std::string( hostname );
    }
    else {
        return std::string("localhost");
    }
#else
    //! @todo implement getHostname in win
    return std::string("localhost");
#endif
}

std::string 
pathDelimeter()
{
#ifndef WIN32 
    return std::string("/");
#else
    return std::string("\\");
#endif
}

} // namespace
