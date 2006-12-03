/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */
#ifndef ORCA2_MISC_STRING_PARSE_UTILS_H
#define ORCA2_MISC_STRING_PARSE_UTILS_H

#include <string>

namespace orcamisc 
{

//! Parses a string returned by Ice::Connection to extract the local address
std::string connectionToLocalAddress( const std::string & s );

//! Parses a string returned by Ice::Connection to extract the remote address
std::string connectionToRemoteAddress( const std::string & s );

//! Parses the stringified proxy of an IceGrid well-known object, e.g. MyGrid/Locator
//! and extracts just the IceGrid Instance Name, e.g. MyGrid.
std::string stringToIceGridInstanceName( const std::string & s );

} // namespace

#endif
