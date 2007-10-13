/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_DYNAMICLOAD_DLL_H
#define ORCA_DYNAMICLOAD_DLL_H

#include <string>

namespace orcadynamicload 
{

//!
//! Class to encapsulate a dynamically-loaded library, so you can load
//! code at run-time.
//!
//! Note that you have to keep this thing in scope for as long as you
//! want to access code from it.
//!
class DynamicallyLoadedLibrary
{
public:

    //! Loads the lib, throws DynamicLoadExceptions on error.
    DynamicallyLoadedLibrary( const std::string &libName );
    ~DynamicallyLoadedLibrary();

    //! Library name
    const std::string &name() const { return libName_; }

    //! Library handle
    void *handle() { return libHandle_; }

private:

    void        *libHandle_;
    std::string  libName_;
};

} // namespace

#endif
