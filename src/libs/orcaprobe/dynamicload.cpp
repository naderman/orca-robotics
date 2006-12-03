/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "dynamicload.h"
#include <iostream>

using namespace std;

namespace orcaprobe {

DynamicallyLoadedLibrary::DynamicallyLoadedLibrary( const std::string &libName )
    : libName_(libName)
{
    libHandle_ = dlopen( libName.c_str(), RTLD_NOW );
    if ( libHandle_ == NULL )
    {
        stringstream ss;
        ss << "Failed to load '"<<libName<<"': "<<dlerror();
        throw DynamicLoadException( ss.str() );
    }
}

DynamicallyLoadedLibrary::~DynamicallyLoadedLibrary()
{
    dlclose( libHandle_ );
}

}
