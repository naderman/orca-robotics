/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_DYNAMICLOAD_DLL_H
#define HYDRO_DYNAMICLOAD_DLL_H

#include <string>

namespace hydrodll 
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
