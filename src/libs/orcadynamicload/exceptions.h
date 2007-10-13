/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCA_DYNAMICLOAD_EXCEPTIONS_H
#define ORCA_DYNAMICLOAD_EXCEPTIONS_H

#include <string>

namespace orcadynamicload {

//! Dynamic load exception
class DynamicLoadException : public std::exception
{ 
public:
    //! Constructor
    DynamicLoadException(const char *message)
        : message_(message) {}
    //! Constructor
    DynamicLoadException(const std::string &message)
        : message_(message) {}
    ~DynamicLoadException()throw(){}
    //! Returns the error message.
    virtual const char* what() const throw() { return message_.c_str(); }
private:
    std::string  message_;
};

} // namespace

#endif
