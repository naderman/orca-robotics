/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2007-2009 Tom Burdick <thomas.burdick@gmail.com>
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDRO_GLU_EXCEPTIONS_H
#define HYDRO_GLU_EXCEPTIONS_H

#include <gbxutilacfr/exceptions.h>

namespace hydroglu
{

//! @brief Raised when a uniform variable in a shader can't be found
class UniformNotFoundException : public gbxutilacfr::Exception
{
public:
    UniformNotFoundException(const char *file, const char *line, const char *message)
        : Exception( file, line, message ) {};
    UniformNotFoundException(const char *file, const char *line, const std::string &message)
        : Exception( file, line, message ) {};
};

//! @brief Raised with a glsl shader cannot be compiled correctly
class CompileException : public gbxutilacfr::Exception
{
public:
    CompileException(const char *file, const char *line, const char *message)
        : Exception( file, line, message ) {};
    CompileException(const char *file, const char *line, const std::string &message)
        : Exception( file, line, message ) {};
};

//! @brief Raised with a glsl shader  program cannot be linked correctly
class LinkException : public gbxutilacfr::Exception
{
public:
    LinkException(const char *file, const char *line, const char *message)
        : Exception( file, line, message ) {};
    LinkException(const char *file, const char *line, const std::string &message)
        : Exception( file, line, message ) {};
};

//! @brief Raised when a glsl file cannot be found
class FileNotFoundException : public gbxutilacfr::Exception
{
public:
    FileNotFoundException(const char *file, const char *line, const char *message)
        : Exception( file, line, message ) {};
    FileNotFoundException(const char *file, const char *line, const std::string &message)
        : Exception( file, line, message ) {};
};

//! @brief Raised when a glsl file cannot be read correctly
class FileReadException : public gbxutilacfr::Exception
{
public:
    FileReadException(const char *file, const char *line, const char *message)
        : Exception( file, line, message ) {};
    FileReadException(const char *file, const char *line, const std::string &message)
        : Exception( file, line, message ) {};
};

}

#endif
