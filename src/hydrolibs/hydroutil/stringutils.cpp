/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#include "stringutils.h"
#include <gbxutilacfr/exceptions.h>
#include <sstream>
#include <iostream>
#include <iomanip> // for setw()

using namespace std;

namespace hydroutil
{

std::vector<std::string>
toStringSeq( const std::string& s, const char delim )
{
    std::string::size_type beg;
    std::string::size_type end = 0;
    const std::string whitespace = " \t\n\r";

    vector<string> seq;
    while( end < s.length() )
    {
        // skip leading white space
        beg = s.find_first_not_of(whitespace, end);
        if(beg == std::string::npos) {
            break;
        }

        // find the delimeter
        end = s.find( delim, beg );
        if(end == std::string::npos) {
            end = s.length();
        }

        // empty string in the sequence, i.e. "something: :else"
        if(end == beg) {
            ++end;
            continue;
        }

        seq.push_back( s.substr(beg, end - beg) );
        ++end;
    }

    return seq;
}

std::string
toString( const std::vector<std::string>& seq, const char delim )
{
    std::string s;

    // check for empty sequence
    if ( seq.empty() )
        return s;

    s = seq[0];

    // append the rest of the sequence with delimeters
    // (this is safe even when size=1: in that case we'll never get inside that loop)
    for ( size_t i=1; i<seq.size(); ++i ) {
        s += delim + seq[i];
    }
    return s;
}

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

int toIntVector( const std::string& s, std::vector<int>& obj )
{
    std::istringstream ss( s );
    
    //check that there's something in the input stream
    if ( !ss )
    {
        return -1;
    }
    
    int tmp;
    while ( true )
    {
        ss >> tmp;
        
        // if we're not at the end of the line but something went wrong, return error
        if ( !ss && !ss.eof() )
        {
            return -1;
        }

        obj.push_back( tmp );
    
        // check if we're at the end of the line
        if ( ss.eof() )
        {
            break;
        }
    }
    
    // success
    return 0;
}

int toDoubleVector( const std::string& s, std::vector<double>& obj )
{
    std::istringstream ss( s );
    
    //check that there's something in the input stream
    if ( !ss )
    {
        return -1;
    }
    
    double tmp;
    while ( true )
    {
        ss >> tmp;
        
        // if we're not at the end of the line but something went wrong, return error
        if ( !ss && !ss.eof() )
        {
            return -1;
        }
        
        obj.push_back( tmp );
    
        // check if we're at the end of the line
        if ( ss.eof() )
        {
            break;
        }
    }
    
    // success
    return 0;
}

void 
substitute( string& v, const vector<string>& parameters, 
            const map<string,string>& values, const map<string,string>& defaults )
{
    string::size_type beg = 0;
    string::size_type end;

    while( (beg = v.find("${", beg)) != string::npos && beg < v.size()-1 )
    {
//         cout<<"Working on string: '"<<v<<"'"<<endl;

        end = v.find( "}" );
        if(end == string::npos) {
            break;
        }
        string variable = v.substr( beg+2, end-beg-2 );

        // check against known parameters
        bool isParameterKnown = false;
        for ( size_t i=0; i<parameters.size(); ++i ) {
            if ( variable == parameters[i] ) {
                isParameterKnown = true;
                break;
            }
        }
        if ( !isParameterKnown )
            throw gbxutilacfr::Exception(ERROR_INFO, "Unknown parameter '"+variable+"'" );

        string value;
        bool isValueKnown = false;

        // first, look for a provided value
        map<string,string>::const_iterator it = values.find( variable );
        if ( it != values.end() ) {
            value = it->second;
            isValueKnown = true;
        }

        // if value is not provided, look for a default value
        if ( !isValueKnown ) {
            map<string,string>::const_iterator it = defaults.find( variable );
            if ( it != defaults.end() ) {
                value = it->second;
                isValueKnown = true;
            }
        }

        if ( !isValueKnown )
            throw gbxutilacfr::Exception(ERROR_INFO, "Unknown value for parameter '"+variable+"'" );

//         cout<<"replacing '"<<v.substr(beg,end-beg+1)<<"' with '"<<value<<"'"<<endl;
        v.replace( beg, end-beg+1, value );

        // Don't advance the position of 'beg', i.e. check the string which we just
        // substituted. This allows recursive substitution.
    }
};

std::string 
toFixedWidth( const std::string& s, int width, char filler, bool adjustLeft )
{
    if ( width<0 )
        return s;

    if ( s.size()<(unsigned int)width ) {
        // need padding
        stringstream ss;
        if ( adjustLeft ) 
            ss<<std::left;

        ss<<std::setfill(filler)<<std::setw(width)<<s;
        return ss.str();
    } 
    else if ( s.size()>(unsigned int)width ) {
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
    return std::string(PROJECT_VERSION);
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
