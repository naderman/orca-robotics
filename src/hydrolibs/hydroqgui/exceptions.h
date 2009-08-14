/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAGUI_EXCEPTIONS_H
#define ORCAGUI_EXCEPTIONS_H

#include <string>
#include <exception>
#include <gbxutilacfr/exceptions.h>

namespace hydroqgui
{
    
//!
//! Exceptions thrown by Gui-related stuff
//!
class Exception : public gbxutilacfr::Exception
{
public:

    Exception(const char *file, const char *line, const char *message)
        : gbxutilacfr::Exception(file,line,message) {}
    Exception(const char *file, const char *line, const std::string &message)
        : gbxutilacfr::Exception(file,line,message) {}

    virtual ~Exception() throw() {}
};

}


#endif
