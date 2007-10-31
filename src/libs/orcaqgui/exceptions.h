/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_EXCEPTIONS_H
#define ORCAGUI_EXCEPTIONS_H

#include <string>
#include <exception>

namespace orcaqgui
{
    
//!
//! Exceptions thrown by Gui-related stuff
//!
//! Can explicitly catch 'orcaqgui::Exception', or just
//! catch all 'std::exception's.
//!
class Exception : public std::exception
{
public:

    Exception(const char *message)
        : message_(message) {}
    Exception(const std::string &message)
        : message_(message) {}

    virtual ~Exception() throw() {}

    virtual const char* what() const throw() { return message_.c_str(); }

protected:

    std::string  message_;
};

}


#endif
