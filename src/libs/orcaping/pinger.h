/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCA2_LIBORCAPING_PINGER_H
#define ORCA2_LIBORCAPING_PINGER_H

#include <sys/types.h> // required because oping doesn't include it.
extern "C" {
#include <oping.h>
}
#include <string>

namespace orcaping {

//!
//! Exception thrown by Pinger.
//!
class PingException : public std::exception
{ 
public:
    PingException(const char *message)
        : message_(message) {}
    PingException(const std::string &message)
        : message_(message) {}
    ~PingException()throw(){}
    virtual const char* what() const throw() { return message_.c_str(); }
protected:
    std::string  message_;
};

//!
//! @author Alex Brooks
//!
class Pinger
{

public: 

    Pinger( double maxTimeoutSec=1 );
    ~Pinger();

    //
    // Returns:
    //   >=0 : a latency in milliseconds
    //   <0  : ping timed out.
    // 
    // May throw exceptions.
    double ping( const std::string &host );

private: 

    pingobj_t *pingObj_;
};

}

#endif
