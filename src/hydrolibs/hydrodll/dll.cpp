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

#ifndef WIN32
#   include <dlfcn.h>
#else
#   include <windows.h>
#endif

#include <hydrodll/dll.h>
#include <hydrodll/exceptions.h>

using namespace std;

namespace hydrodll {

DynamicallyLoadedLibrary::DynamicallyLoadedLibrary( const std::string &libName )
    : libName_(libName)
{
#ifndef WIN32
    libHandle_ = dlopen( libName.c_str(), RTLD_NOW );
	if ( libHandle_ == NULL )
    {
        stringstream ss;
        ss << "Failed to load '"<<libName<<"': "<<dlerror();
      
		throw DynamicLoadException( ss.str() );
    }
#else
	libHandle_ = LoadLibrary( libName.c_str() );
	if ( libHandle_ == NULL )
	{
		stringstream ss;
		ss << "Failed to load'"<<libName<<"': " << GetLastError();
		throw DynamicLoadException( ss.str() );
	}
#endif

}

DynamicallyLoadedLibrary::~DynamicallyLoadedLibrary()
{
#ifndef WIN32
    dlclose( libHandle_ );
#else
	FreeLibrary( (HMODULE)libHandle_ );
#endif
}

}
