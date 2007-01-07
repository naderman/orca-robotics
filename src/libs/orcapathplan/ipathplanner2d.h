/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef IPATHPLANNER2D_H
#define IPATHPLANNER2D_H

#include <orcapathplan/pathplanutils.h>

namespace orcapathplan {

//!
//! Exceptions thrown by Path Planners.
//!
//! Can explicitly catch 'orcapathplan::Exception', or just
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

    virtual Result type() const { return OtherError; }
    
protected:

    std::string  message_;
};

class PathStartNotValidException : public Exception
{
    public:
        PathStartNotValidException(const char *message)
            : Exception( message)
        {}
        virtual Result type() const { return PathStartNotValid; }
};

class PathDestinationNotValidException : public Exception
{
    public:
        PathDestinationNotValidException(const char *message)
            : Exception( message)
        {}
        virtual Result type() const { return PathDestinationNotValid; }
};

class PathDestinationUnreachabledException : public Exception
{
    public:
        PathDestinationUnreachabledException(const char *message)
            : Exception( message)
        {}
        virtual Result type() const { return PathDestinationUnreachable; }
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
    // Warning: may throw 'orcapathplan::Exception's
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
