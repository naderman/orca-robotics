/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICEUTIL_STRING_UTILITIES_H
#define ORCAICEUTIL_STRING_UTILITIES_H

#include <string>
#include <vector>

namespace orcaiceutil
{

/*!
 *  @name String Utilities
 */
//@{

//! Parses the string into a sequence of strings with a given separator.
std::vector<std::string> toStringSeq( const std::string& s, const char delim=':' );
//! Combines the sequence of strings into a single string using a given separator.
std::string toString( const std::vector<std::string>& seq, const char delim=':' );

//! Converts the whole string to lower case.
std::string toLowerCase( const std::string& s );

//! Converts the whole string to upper case.
std::string toUpperCase( const std::string& s );

//! Unlike std::setw() function, this functions pads and truncates.
//! When width=0, an empty string is returned.
//! When width<0, the string is quietly returned unmodified.
//! Default fill character is a space.
std::string toFixedWidth( const std::string& s, int width, char filler=' ', 
                            bool adjustLeft=false );

//! Returns version of libOrcaIce, e.g. "3.2.1"
std::string orcaVersion();

//! Similar to UNIX basename command. Removes the path, i.e. all leading
//! characters up to the path delimeter ('/' in Linux, '\' in Windows).
//! E.g. basename("/path/filename.ext") returns "filename.txt".
//! Unlike the Unix equivalent, this function returns an empty string 
//! when the intput string is a directory name.
//! E.g. basename("/path/" ) returns "".
//! If the optional flag removeExtension is set to TRUE, then all trailing
//! characters following and including the last '.' are removed as well.
//! E.g. basename("/path/filename.ext", true) returns "filename".
std::string basename( const std::string& path, bool removeExtension=false );

//! Similar to UNIX dirname command. Removes the trailing characters following
//! and including the last path delimeter ('/' in Linux, '\' in Windows).
//! E.g. dirname("/path/filename.ext") returns "/path".
//! If the path delimeters are absent, returns ".".
std::string dirname( const std::string& path );

//@}
} // namespace

#endif
