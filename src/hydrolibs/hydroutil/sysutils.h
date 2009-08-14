/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDROUTIL_SYSTEM_UTILITIES_H
#define HYDROUTIL_SYSTEM_UTILITIES_H

#include <string>

namespace hydroutil
{
/*!
 *  @name System-Dependent Functions
 */
//@{

/*!
 *  Platform-independent function to get the hostname.
 *  On error, returns @e localhost.
 *
 *  @note only Linux version is implemented, under Windows returns @e localhost.
 */
std::string getHostname();

//! Returns "/" in Linux, "\" in Windows.
std::string pathDelimeter();

//! Returns:
//!   - True:  success
//!   - False: failure (and failReason is set)
//!
//! On Linux calls popen : opens a process by creating a pipe, forking, and invoking the shell 
//! (see: man popen). Grabs both stdout and stderr.
//!
bool executeSystemCommand( const std::string &command, std::string &failReason, std::string *output=NULL );


//@}
} // namespace

#endif
