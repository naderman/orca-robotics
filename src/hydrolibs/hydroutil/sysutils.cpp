/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include <iostream>
#include <sstream>
#include <stdlib.h> // for getenv()
#include <stdio.h>
#include <assert.h>

#include "sysutils.h"
#ifndef WIN32
#   include <unistd.h>
#endif 

using namespace std;

namespace hydroutil
{

string 
getHostname()
{
#ifndef WIN32 
    char hostname[256];
    int ret = gethostname(hostname,256);
    if ( ret==0 ) {
        return string( hostname );
    }
    else {
        return string("localhost");
    }
#else
    //! @todo implement getHostname in win
    return string("localhost");
#endif
}

string 
pathDelimeter()
{
#ifndef WIN32 
    return string("/");
#else
    return string("\\");
#endif
}

bool 
executeSystemCommand( const string &command, string &failReason, string *output )
{
#ifndef WIN32 
    // Grab stderr too.
    string fullCommand = command + " 2>&1";

    FILE *fp = popen( fullCommand.c_str(), "r" );
    if ( fp <= 0 )
    {
        failReason = "popen failed.";
        return false;
    }

// LINE_MAX is missing when compiling with gcc-4.3. Does the array size matter here?
#ifndef LINE_MAX
# define LINE_MAX 1024
#endif

    stringstream outputSS;
    char line[LINE_MAX];
    while (fgets(line, LINE_MAX, fp) != NULL) 
    {
        outputSS << line;
    }
    
    int closeStatus = pclose( fp );

    int exitStatus = WEXITSTATUS(closeStatus);

    if ( exitStatus != 0 )
    {
        stringstream ss;
        ss << command << " failed with exit status " << exitStatus << ".  Output was: " << endl << outputSS.str();
        failReason = ss.str();
        return false;
    }

    if ( output != NULL )
        *output = outputSS.str();

    return true;
#else
    failReason = "executeSystemCommand() is not implemented in WIN";
    return false;
#endif
}

} // namespace
