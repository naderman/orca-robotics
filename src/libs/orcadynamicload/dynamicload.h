/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_DYNAMICLOAD_DYNAMICLOAD_H
#define ORCA_DYNAMICLOAD_DYNAMICLOAD_H

#include <orcadynamicload/dll.h>
#include <orcadynamicload/exceptions.h>
#include <dlfcn.h>
#include <string>
#include <sstream>

namespace orcadynamicload 
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
    void *makerFuncHandle = dlsym(lib.handle(),makerFuncName);
    if ( makerFuncHandle == NULL )
    {
        std::stringstream ss;
        ss << "Failed to find symbol '"<<makerFuncName<<"' in library '"<<lib.name()<<"': "<<dlerror();
        throw DynamicLoadException( ss.str() );
    }

    MakerFunc *makerFunc = (MakerFunc*)makerFuncHandle;

    LoadedClass *loadedClass = makerFunc();
    return loadedClass;
};

} // namespace

#endif
