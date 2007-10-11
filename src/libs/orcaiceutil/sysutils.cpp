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
#include <sstream>
#include <stdlib.h> // for getenv()
#include <assert.h>

#include "sysutils.h"
#include "exceptions.h"
#ifndef WIN32
#   include <unistd.h>
#endif 

using namespace std;

namespace orcaiceutil
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
    assert( false && failReason );
    return false;
#endif
}

} // namespace
