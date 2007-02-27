/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include <iostream> // debug only
#include <sstream>

#include "exceptions.h"
#include "utils.h"

using namespace std;

void 
orcalog::parseRequiredTag( const std::string& tag, std::string& type, std::string& suffix )
{
    if ( tag.empty() ) {
        throw orcalog::Exception( ERROR_INFO, "empty interface tag" );
    }

    // make a copy
    type = tag;

    // strip numbers from the back
    // assume: no interface names end in numbers.
    while ( type.size() > 0 &&
            type[type.size()-1] >= '0' &&
            type[type.size()-1] <= '9' )
    {
        type.resize( type.size()-1 );
    }

    if ( type.empty() ) {
        throw orcalog::Exception( ERROR_INFO, "found empty type in interface tag "+tag );
    }

    // now we should have tag = type + suffix
    int suffixSize = tag.size() - type.size();

    if ( suffixSize == 0 ) {
        throw orcalog::Exception( ERROR_INFO, "found empty type suffix in interface tag "+tag );
    }

    suffix = tag.substr( tag.size()-suffixSize, suffixSize );

    // debug
//     cout<<"tag="<<tag<<" type="<<type<<" sfx="<<suffix<<" ("<<suffixSize<<")"<<endl;
 }

std::string 
orcalog::headerLine( const std::string& filename, 
                    const std::string&  interfaceType, 
                    const std::string&  format,
                    bool                enabled )
{
    stringstream ss;
    ss << filename << " " << interfaceType << " " << format << " " << enabled;
    return ss.str();
}

void 
orcalog::parseHeaderLine( const std::string& line, 
                        std::string& filename, 
                        std::string& interfaceType, 
                        std::string& format,
                        bool&        enabled )
{
    stringstream ss( line );
    ss >> filename >> interfaceType >> format >> enabled;
}

std::string 
orcalog::endOfHeader()
{
    return "#data";
}

bool 
orcalog::isEndOfHeader( const std::string& line )
{
    return ( line == "#data" );
}

std::string
orcalog::dataLine( int seconds, int useconds, int id, int index )
{
    stringstream ss;
    ss << seconds << " " << useconds << " " << id << " " << index;
    return ss.str();
}

void
orcalog::parseDataLine( const std::string& line, int& seconds, int& useconds, int& id, int& index )
{
    if ( line.empty() ) {
        throw orcalog::Exception( ERROR_INFO, "data line string is empty." );
    }

    stringstream ss( line );
    ss.exceptions( istream::failbit );
    try
    {
        ss >> seconds >> useconds >> id >> index;
    }
    catch ( const std::exception& )
    {
        throw orcalog::Exception( ERROR_INFO, "error parsing data line '"+line+"'." );
    }
}
