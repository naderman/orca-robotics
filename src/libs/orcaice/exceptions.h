/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_EXCEPTIONS_H
#define ORCAICE_EXCEPTIONS_H

/*
 * STRINGIZE macro converts an expression into a string-literal.
 * ERROR_INFO macro permits file-name and line-number data to be added to an error message.
 *
 * Adapted by Alex Brooks from Tim Bailey's version 2005.
 */

#ifndef ERROR_MACROS_HPP_
#define ERROR_MACROS_HPP_

#if defined(STRINGIZE_HELPER) || defined(STRINGIZE) || defined(ERROR_INFO)
#   error OrcaIceUtil error macros have already been defined elsewhere 
#endif

#define STRINGIZE_HELPER(exp) #exp
#define STRINGIZE(exp) STRINGIZE_HELPER(exp)

#define ERROR_INFO __FILE__, STRINGIZE(__LINE__)

#endif

#include <exception>
#include <string>

namespace orcaice
{

//!
//! @brief Base class for all libOrcaIce exceptions.
//!
//! Generally, this exception should be thrown like so:
//!
//!  - throw orcaice::Exception( ERROR_INFO, "description of error" );
//!
//! where the ERROR_INFO macro inserts the offending file and line number.
//!
class Exception : public std::exception
{
public:

    Exception(const char *file, const char *line, const char *message);
    Exception(const char *file, const char *line, const std::string &message);

    virtual ~Exception() throw();

    virtual const char* what() const throw() { return message_.c_str(); }

protected:

    void setMsg( const char *file, const char *line, const char *message );

    std::string  message_;

private:
    const char *basename( const char *s );
};

//! @brief This exception is raised when something goes wrong while reading the configuration file.
//! 
//! Potential problems include file missing, the specified property tag missing, or the property
//! value is of wrong type.
class ConfigFileException : public orcaice::Exception
{
public:
    ConfigFileException(const char *file, const char *line, const char *message)
            : Exception( file, line, message ) {};
    ConfigFileException(const char *file, const char *line, const std::string &message)
            : Exception( file, line, message ) {};
};

//! This exception is raised when something is wrong with the hardware.
class HardwareException : public orcaice::Exception
{
public:
    HardwareException(const char *file, const char *line, const char *message)
            : Exception( file, line, message ) {};
    HardwareException(const char *file, const char *line, const std::string &message)
            : Exception( file, line, message ) {};
};

//! This exception is raised when something is wrong with the network connection.
class NetworkException : public orcaice::Exception
{
    public:
        NetworkException(const char *file, const char *line, const char *message)
            : Exception( file, line, message ) {};
        NetworkException(const char *file, const char *line, const std::string &message)
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
