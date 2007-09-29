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

//@}
} // namespace

#endif
