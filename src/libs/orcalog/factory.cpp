/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "factory.h"

#include <iostream>

using namespace std;
using namespace orcalog;

std::vector<std::string> 
Factory::supportedTypes() const
{
    return types_;
}

bool 
Factory::isSupported( const std::string &interfaceType ) const
{
    for ( unsigned int i=0; i<types_.size(); ++i ) {
        cout << "types_[i], interfaceType: " << types_[i] <<", " << interfaceType << endl;
        if ( types_[i] == interfaceType ) {
            return true;
        }
    }
    return false;
}

void
Factory::addSupportedType( const std::string & interfaceType )
{
    types_.push_back( interfaceType );
}
