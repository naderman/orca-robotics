#ifndef USBFTDI_EXCEPTIONS_H
#define USBFTDI_EXCEPTIONS_H

#include <ftd2xx.h>
#include <exception>
#include <string>

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


namespace usbftdi
{

//!
//! Exceptions thrown by UsbFtdi
//!
//! Generally, this exception should be thrown like so:
//!
//!  - throw Exception( ERROR_INFO, "description of error" );
//!  - throw Exception( ERROR_INFO, "description of error", ftStatus );
//!
//! where the ERROR_INFO macro inserts the offending file and line number.
//!
class Exception : public std::exception
{
public:

    Exception(const char *file, const char *line, const char *message);
    Exception(const char *file, const char *line, const std::string &message);
    Exception(const char *file, const char *line, const char *message, FT_STATUS ftStatus );
    Exception(const char *file, const char *line, const std::string &message, FT_STATUS ftStatus);

    virtual ~Exception() throw();

    virtual const char* what() const throw() { return message_.c_str(); }

protected:

    void setMsg( const char *file, const char *line, const char *message );
    const char *basename( const char *s );

    std::string  message_;
};

    // Returns a human-readable status string
    std::string ftStatusToString( FT_STATUS status );

}

#endif
