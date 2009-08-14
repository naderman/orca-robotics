/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDROPING_PINGER_H
#define HYDROPING_PINGER_H

#include <sys/types.h> // required because oping doesn't include it.
extern "C" {
#include <oping.h>
}
#include <string>
#include <gbxutilacfr/exceptions.h>

namespace hydroping {

//!
//! Exception thrown by Pinger.
//!
class PingException : public gbxutilacfr::Exception
{ 
public:
    //! Constructor
    PingException(const char *file, const char *line, const char *message)
        : gbxutilacfr::Exception(file,line,message) {}
    //! Constructor
    PingException(const char *file, const char *line, const std::string &message)
        : gbxutilacfr::Exception(file,line,message) {}
    ~PingException()throw(){}
};

//!
//! @author Alex Brooks
//!
class Pinger
{

public: 
    //! Constructor
    Pinger( double maxTimeoutSec=1 );
    ~Pinger();

    //!
    //! Returns:
    //! - >=0 : a latency in milliseconds
    //! - <0  : ping timed out.
    //! 
    //! May throw PingException.
    double ping( const std::string &host );

private: 

    pingobj_t *pingObj_;
};

}

#endif
