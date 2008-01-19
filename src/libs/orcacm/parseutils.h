/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */
#ifndef ORCACM_MISC_STRING_PARSE_UTILS_H
#define ORCACM_MISC_STRING_PARSE_UTILS_H

#include <string>

namespace orcacm 
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
