/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_DYNAMICLOAD_DYNAMICLOAD_H
#define HYDRO_DYNAMICLOAD_DYNAMICLOAD_H

#include <hydrodll/dll.h>
#include <hydrodll/exceptions.h>
#ifndef WIN32
#include <dlfcn.h>
#else
#include <windows.h>
#endif
#include <string>
#include <sstream>

namespace hydrodll 
{

//!
//! Uses a function in the lib to load an instance of 'LoadedClass'.
//!
//! Assumes the library has a 'maker' function with the signature:
//!   extern "C" {
//!     LoadedClass *makerFunc();
//!   }
//!
//! The type of this function should be typedef'd to MakerFunc somewhere, as in:
//!   typedef LoadedClass *MakerFunc();
//! 
//!
template<class LoadedClass, typename MakerFunc>
LoadedClass *dynamicallyLoadClass( DynamicallyLoadedLibrary &lib, const char *makerFuncName )
{
#ifndef WIN32
    void *makerFuncHandle = dlsym(lib.handle(),makerFuncName);
    if ( makerFuncHandle == NULL )
    {
        std::stringstream ss;
        ss << "Failed to find symbol '"<<makerFuncName<<"' in library '"<<lib.name()<<"': "<<dlerror();
        throw DynamicLoadException( ss.str() );
    }
#else
    void *makerFuncHandle = (void*)GetProcAddress((HMODULE)lib.handle(),makerFuncName);
    if ( makerFuncHandle == NULL )
    {
        std::stringstream ss;
        ss << "Failed to find symbol '"<<makerFuncName<<"' in library '"<<lib.name()<<"': "<<GetLastError();
        throw DynamicLoadException( ss.str() );
    }
#endif

    MakerFunc *makerFunc = (MakerFunc*)makerFuncHandle;

    LoadedClass *loadedClass = makerFunc();
    return loadedClass;
};

} // namespace

#endif
