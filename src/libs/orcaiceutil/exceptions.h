/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICEUTIL_EXCEPTIONS_H
#define ORCAICEUTIL_EXCEPTIONS_H

/*
 * STRINGIZE macro converts an expression into a string-literal.
 * ERROR_INFO macro permits file-name and line-number data to be added to an error message.
 *
 * Adapted by Alex Brooks from Tim Bailey's version 2005.
 */

#ifndef ERROR_MACROS_HPP_
#define ERROR_MACROS_HPP_

#if defined(STRINGIZE_HELPER) || defined(STRINGIZE) || defined(ERROR_INFO)
#   error OrcaIce error macros have already been defined elsewhere 
#endif

#define STRINGIZE_HELPER(exp) #exp
#define STRINGIZE(exp) STRINGIZE_HELPER(exp)

#define ERROR_INFO __FILE__, STRINGIZE(__LINE__)

#endif

#include <exception>
#include <string>

namespace orcaiceutil
{

/*!
@brief Base class for all Orca exceptions.

Generally, this exception should be thrown like so:

@verbatim
throw orcaiceutil::Exception( ERROR_INFO, "description of error" );
@endverbatim

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

    std::string  message_;

private:
    const char *basename( const char *s );
};

} // namespace


#endif
