/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef LOCKFILE_H
#define LOCKFILE_H

#include <exception>
#include <string>

namespace orcaserial {

//!
//! @brief Exception thrown by lockfile functions
//!
class LockfileException : public std::exception
{ 
    std::string  message_;
public:
    LockfileException(const char *message)
        : message_(message) {}
    LockfileException(const std::string &message)
        : message_(message) {}
    ~LockfileException()throw(){}
    virtual const char* what() const throw() { return message_.c_str(); }
};

//!
//! Creates a lock-file which can be used to prevent multiple access to
//! a device (ie /dev/xxx).
//! Stores the PID of the locking process (lockPid), so it can check for stale lock-files.
//! Throws LockfileException's on errors (including 'device locked').
//!
//! The destructor removes the lock-file (guarantees that no exceptions are thrown).
//!
//! For more info, see: http://tldp.org/HOWTO/Serial-HOWTO-14.html
//!
class LockFile {
public:

    LockFile( const std::string &dev,
              int lockPid = getpid() );
    ~LockFile();

private:

    const std::string dev_;
    const int lockPid_;

};

}

#endif
