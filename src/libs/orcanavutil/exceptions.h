/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>

namespace orcanavutil {

//!
//! @brief Exception thrown by orcanavutil stuff.
//!
//! @author Alex Brooks
//!
class Exception : public std::exception
{ 
    std::string  message_;
public:
    Exception(const char *message)
        : message_(message) {}
    Exception(const std::string &message)
        : message_(message) {}
    ~Exception()throw(){}
    virtual const char* what() const throw() { return message_.c_str(); }
};

}

#endif
