/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef IPATHPLANNER2D_H
#define IPATHPLANNER2D_H

#include <hydropathplan/util.h>
#include <gbxutilacfr/exceptions.h>

namespace hydropathplan {

//!
//! Base class for exceptions thrown by Path Planners.
//!
class Exception : public gbxutilacfr::Exception
{
public:
    
    Exception( const char *file, const char *line, const std::string &message )
        : gbxutilacfr::Exception( file, line, message ) {}    
    virtual ~Exception() throw() {}

    //! Used for keeping the type of the exception, but adding more information
    void prependMsg( const char *file, const char *line, const std::string &msg )
        { message_ = gbxutilacfr::Exception::toMessageString(file,line,msg) + message_; }
protected:
    std::string  message_;
};

class PathStartNotValidException : public Exception
{
public:
    PathStartNotValidException( const char *file, const char *line, const std::string &message )
        : Exception( file, line, std::string("PathStartNotValid: ")+message ) {}
};

class PathDestinationNotValidException : public Exception
{
public:
    PathDestinationNotValidException( const char *file, const char *line, const std::string &message )
        : Exception( file, line, std::string("PathDestinationNotValid: ")+message ) {}
};

class PathDestinationUnreachableException : public Exception
{
public:
    PathDestinationUnreachableException( const char *file, const char *line, const std::string &message )
        : Exception( file, line, std::string("PathDestinationUnreachable: ")+message ) {}
};

//!
//! Abstract interface for a 2D path planner
//!
//! @author Alex Brooks
//!
class IPathPlanner2d
{

public: 

    virtual ~IPathPlanner2d() {};

    //
    // Compute the path from 'start' to 'end'.
    // NOTE: Everything is done in cells (not world-coordinates).
    //
    // The computed path is not necessarily a set of contiguous cells.
    //
    // Warning: may throw 'hydropathplan::Exception's
    //
    virtual void computePath( int           startX,
                              int           startY,
                              int           endX,
                              int           endY,
                              Cell2DVector &path ) const = 0;

private: 

};

}

#endif
