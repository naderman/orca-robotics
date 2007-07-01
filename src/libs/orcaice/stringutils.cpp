/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "stringutils.h"
#include <sstream>
#include <iomanip> // for setw()

using namespace std;

namespace orcaice
{

std::string
toLowerCase( const std::string & s )
{
    std::string s2 = s;
    for ( unsigned i = 0; i < s2.size(); ++i ) {
        s2[i] = tolower( s2[i] );
    }
    return s2;
}

std::string
toUpperCase( const std::string & s )
{
    std::string s2 = s;
    for ( unsigned i = 0; i < s2.size(); ++i ) {
        s2[i] = toupper( s2[i] );
    }
    return s2;
}

std::string 
toFixedWidth( const std::string& s, unsigned int width, char filler, bool adjustLeft )
{
    if ( width<0 )
        return s;

    if ( s.size()<width ) {
        // need padding
        stringstream ss;
        if ( adjustLeft ) 
            ss<<std::left;

        ss<<std::setfill(filler)<<std::setw(width)<<s;
        return ss.str();
    } 
    else if ( s.size()>width ) {
        // need trunkating
        return s.substr(0,width);
    } 
    else {
        // needs no change
        return s;
    }
}

std::string 
orcaVersion()
{
    return std::string(ORCA_VERSION);
}

std::string 
basename( const std::string & path, bool removeExtension )
{
    string filename;

    // path delimeters are OS-dependent.
#ifndef WIN32 
    char delim = '/';
#else
    char delim = '\\';
#endif
    // remove leading path
    filename = path.substr( path.find_last_of( delim )+1 );

    if ( removeExtension ) {
        filename = filename.substr( 0, filename.find_last_of('.') );
    }
    return filename;
}

std::string 
dirname( const std::string & path )
{
    std::string dir;

    // path delimeters are OS-dependent.
#ifndef WIN32 
    char delim = '/';
#else
    char delim = '\\';
#endif

    // remove trailing filename
    dir = path.substr( 0, path.find_last_of( delim ) );

    // delimiter was not found in the path ==> the path is a filename
    if ( dir == path ) {
        dir = ".";
    }

    return dir;
}

} // namespace
