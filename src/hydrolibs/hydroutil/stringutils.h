/*
 * Orca-Robotics Project: Components for robotics
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This distribution is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef HYDROUTIL_STRING_UTILITIES_H
#define HYDROUTIL_STRING_UTILITIES_H

#include <string>
#include <vector>
#include <map>

namespace hydroutil
{

/*!
 *  @name String Utilities
 */
//@{

//! Parses the string into a sequence of strings with a given separator.
std::vector<std::string> toStringSeq( const std::string& s, const char delim=':' );
//! Combines the sequence of strings into a single string using a given separator.
std::string toString( const std::vector<std::string>& seq, const char delim=':' );
//! Same as above but the vector of strings is sorted first.
std::string toSortedString( const std::vector<std::string>& seq, const char delim=':' );

//! Converts the whole string to lower case.
std::string toLowerCase( const std::string& s );

//! Converts the whole string to upper case.
std::string toUpperCase( const std::string& s );

//! Parses string of int's separated by spaces.
//! Returns: 0 = parsing successful, non-zero = parsing failed.
int toIntVector( const std::string &, std::vector<int>& );

//! Parses string of doubles separated by spaces.
//! Returns: 0 = parsing successful, non-zero = parsing failed.
int toDoubleVector( const std::string &, std::vector<double>& );

/*!
Performs parameter substitution in string the string provided.
Parameter syntax: ${parameter_name}. Valid parameters are given in @c parameters.
Substitution values are first looked up in @c values and then in @c defaults.
Throws gbxutilacfr::Exception if the string conatains an unknown parameter
or if the parameter has no value (provided or default).

Substitution can be recursive, provided that enough information is provided and the
dependencies are not circular. For example @c var1.value="a", @c var2.default=$var1".
Be careful, circular dependencies are not detected. For example, this will lead to an infinite loop:
@c var1.value="$var2", @c var2.value=$var1".

The input string can contain '${' without a matching '}'. These are ignored.

    Example:
@verbatim
string s="The best framework is ${framework}";

vector<string> parameters;
parameters.push_back("software");
parameters.push_back("framework");

map<string,string> values;

map<string,string> defaults;
defaults["framework"] = "Orca";

substitute( s, parameters, values, defaults );

cout<<s<endl; // output: "The best framework is Orca"
@endverbatim
*/
void substitute( std::string& s, const std::vector<std::string>& parameters,
                const std::map<std::string,std::string>& values,
                const std::map<std::string,std::string>& defaults );

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
