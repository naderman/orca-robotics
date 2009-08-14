/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <gbxutilacfr/exceptions.h>

namespace hydronavutil {

//!
//! @brief Exception thrown by hydronavutil stuff.
//!
//! @author Alex Brooks
//!
class Exception : public gbxutilacfr::Exception
{ 
public:
    Exception(const char *file, const char *line, const char *message)
            : gbxutilacfr::Exception( file, line, message ) {};
    Exception(const char *file, const char *line, const std::string &message)
            : gbxutilacfr::Exception( file, line, message ) {};
};    

}

#endif
