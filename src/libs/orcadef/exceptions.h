/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_DEF_EXCEPTIONS_H
#define ORCA_DEF_EXCEPTIONS_H

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

namespace orcadef
{

/*!
    @brief Base class for exceptions coming from libOrcaDef.
*/
/*
    For Developers:
    Generally, this exception should be thrown like so:
    
     - throw Exception( ERROR_INFO, "description of error" );
    
    where the ERROR_INFO macro inserts the offending file and line number.
*/
class Exception : public std::exception
{
public:

    Exception(const char *file, const char *line, const char *message);
    Exception(const char *file, const char *line, const std::string &message);

    virtual ~Exception() throw();

    virtual const char* what() const throw() { return message_.c_str(); }

protected:

    void setMsg( const char *file, const char *line, const char *message );
    const char *basename( const char *s );

    std::string  message_;
};

//! @brief Throw it when something is wrong with reading the definition file.
class FileException : public orcadef::Exception
{
public:
    FileException(const char *file, const char *line, const char *message)
        : Exception( file, line, message ) {};
    FileException(const char *file, const char *line, const std::string &message)
        : Exception( file, line, message ) {};
};

//! @brief Throw it when something is wrong in parsing the definition file.
class ParseException : public orcadef::Exception
{
public:
    ParseException(const char *file, const char *line, const char *message)
        : Exception( file, line, message ) {};
    ParseException(const char *file, const char *line, const std::string &message)
        : Exception( file, line, message ) {};
};

} // namespace


#endif
