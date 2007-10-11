/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICEUTIL_SYSTEM_UTILITIES_H
#define ORCAICEUTIL_SYSTEM_UTILITIES_H

#include <string>

namespace orcaiceutil
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
//! On Linux calls popen : opens a process by creating a pipe, forking, and invoking the shell 
//! (see: man popen).
//!
bool executeSystemCommand( const std::string &command, std::string &failReason, std::string *output=NULL );


//@}
} // namespace

#endif
