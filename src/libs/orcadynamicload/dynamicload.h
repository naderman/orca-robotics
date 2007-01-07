/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_DYNAMIC_LOAD_H
#define ORCA_DYNAMIC_LOAD_H

#include <string>
#include <sstream>
#include <dlfcn.h>

namespace orcadynamicload {

// Exceptions
class DynamicLoadException : public std::exception
{ 
    std::string  message_;
public:
    DynamicLoadException(const char *message)
        : message_(message) {}
    DynamicLoadException(const std::string &message)
        : message_(message) {}
    ~DynamicLoadException()throw(){}
    virtual const char* what() const throw() { return message_.c_str(); }
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


//
// Class to encapsulate a dynamically-loaded library, so you can load
// code at run-time.
//
// Note that you have to keep this thing in scope for as long as you
// want to access code from it.
//
class DynamicallyLoadedLibrary
{
public:

    // Loads the lib, throws DynamicLoadExceptions on error.
    DynamicallyLoadedLibrary( const std::string &libName );
    ~DynamicallyLoadedLibrary();

    const std::string &name() const { return libName_; }
    void *handle() { return libHandle_; }

private:

    void        *libHandle_;
    std::string  libName_;
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////


//
// Uses a function in the lib to load an instance of 'LoadedClass'.
//
// Assumes the library has a 'maker' function with the signature:
//   extern "C" {
//     LoadedClass *makerFunc();
//   }
//
// The type of this function should be typedef'd to MakerFunc somewhere, as in:
//   typedef LoadedClass *MakerFunc();
// 
//
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
}

}

#endif
