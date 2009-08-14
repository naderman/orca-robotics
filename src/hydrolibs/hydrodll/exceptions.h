/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_DYNAMICLOAD_EXCEPTIONS_H
#define HYDRO_DYNAMICLOAD_EXCEPTIONS_H

#include <string>

namespace hydrodll {

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
