/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

// #include <iostream> // debug only
#include <sstream>

#include "utils.h"

// using namespace std;
namespace orcawall
{

void 
parseInterfaceTag( const std::string& tag, std::string& type, std::string& suffix )
{
    if ( tag.empty() ) {
        throw std::string("orcawall::parseInterfaceTag(): empty interface tag");
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
        throw std::string("orcawall::parseInterfaceTag(): found empty type in interface tag "+tag );
    }

    // now we should have tag = type + suffix
    int suffixSize = tag.size() - type.size();

    if ( suffixSize == 0 ) {
        throw std::string("orcawall::parseInterfaceTag(): found empty type suffix in interface tag "+tag );
    }

    suffix = tag.substr( tag.size()-suffixSize, suffixSize );

    // debug
//     cout<<"tag="<<tag<<" type="<<type<<" sfx="<<suffix<<" ("<<suffixSize<<")"<<endl;
 }

} // namespace
