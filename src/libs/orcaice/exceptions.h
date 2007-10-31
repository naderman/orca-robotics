/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_EXCEPTIONS_H
#define ORCAICE_EXCEPTIONS_H

#include <hydroutil/exceptions.h>

namespace orcaice
{

//! @brief This exception is raised when something goes wrong while reading the configuration file.
//! 
//! Potential problems include file missing, the specified property tag missing, or the property
//! value is of wrong type.
class ConfigFileException : public hydroutil::Exception
{
public:
    ConfigFileException(const char *file, const char *line, const char *message)
            : Exception( file, line, message ) {};
    ConfigFileException(const char *file, const char *line, const std::string &message)
            : Exception( file, line, message ) {};
};

//! This exception is raised when something is wrong with the network connection.
class NetworkException : public hydroutil::Exception
{
    public:
        NetworkException(const char *file, const char *line, const char *message)
            : Exception( file, line, message ) {};
        NetworkException(const char *file, const char *line, const std::string &message)
            : Exception( file, line, message ) {};
};

//! This exception is raised when you try to do something like activate the component
//! while the component is in the process of deactivating
class ComponentDeactivatingException : public hydroutil::Exception
{
    public:
        ComponentDeactivatingException(const char *file, const char *line, const char *message)
            : Exception( file, line, message ) {};
        ComponentDeactivatingException(const char *file, const char *line, const std::string &message)
            : Exception( file, line, message ) {};
};


//! @brief This exception is raised if the required interface type does not match the proxy type.
//!
//! In Ice terms, the interface type is called Object Id. It is obtained at runtime by
//! calling ice_id() on the proxy.
class TypeMismatchException : public orcaice::NetworkException
{
    public:
        TypeMismatchException(const char *file, const char *line, const char *message)
            : NetworkException( file, line, message ) {};
        TypeMismatchException(const char *file, const char *line, const std::string &message)
            : NetworkException( file, line, message ) {};
};

} // namespace


#endif
