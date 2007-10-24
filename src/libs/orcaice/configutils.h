/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2007 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_CONFIG_UTILITIES_H
#define ORCAICE_CONFIG_UTILITIES_H


#include <orca/ocm.h>
#include <orcaice/context.h>

// Note: functions in this file don't use the network. Those that do are in connectutils.*

namespace orcaice
{
/*!
 *  @name Component Configuration Functions
 */
//@{

//! Reads properties contained in the context and returns the fully-qualified
//! name of the provided interface corresponding to interfaceTag.
//!
//! Throws ConfigFileException if the interface name cannot be read for some reason.
orca::FQInterfaceName getProvidedInterface( const Context &, const std::string &interfaceTag );

//! Reads platform and component names from the context, and the interface name and 
//! returns the fully-qualified topic name, and the optional subtopic name.
//! We don't check if the interfaceName is empty.
orca::FQTopicName getProvidedTopicWithString( const Context &, const std::string &interfaceName, const std::string & subtopic="*" );

//! Convenience function which behaves like getProvidedTopicWithString() but the interface
//! name is looked up in the config file using the interfaceTag.
//! Throws ConfigFileException if the interface name cannot be read for some reason.
orca::FQTopicName getProvidedTopicWithTag( const Context &, const std::string &interfaceTag, const std::string & subtopic="*" );

//! Reads properties contained in the context and returns the stringified proxy to the 
//! required interface corresponding to the interface tag. 
//!
//! The proxy can be direct or indirect. For indirect proxies with platform name set to @e local, 
//! it is replaced it with hostname.
//! 
//! Throws ConfigFileException if the interface name cannot be read for some reason.
std::string getRequiredInterfaceAsString( const Context &, const std::string &interfaceTag );

//! For components with variable number of interfaces it may be useful to get a listing
//! of all provided interfaces described in the configuration file.
//!
//! See getRequiredTags() for the usage of @c pattern.
std::vector<std::string> getProvidedTags( const Context & context, const std::string& pattern="" );

/*! 
For components with variable number of interfaces it may be useful to get a listing
of all required interfaces described in the configuration file.

You may have a group of permanent required interfaces and group of variable or optional ones. To 
separate the two groups you may want to start all optional tags with a certain @e pattern. 
For example, your config file may look like this:
@verbatim
MyComponent.Requires.Locolise2d.Proxy=something
MyComponent.Requires.LaserScanner2dFront.Proxy=something
MyComponent.Requires.LaserScanner2dBack.Proxy=something
@endverbatim
In this case you would get a list of "laser" required interfaces by supplying the pattern="LaserScanner2d"
or pattern="Laser" or pattern="La". For example:
@verbatim
std::vector<std::string> laserTags;
laserTags = orcaice::getRequiredTags( context_, "Laser" );
@endverbatim
The default empty string pattern will match all required interfaces.
*/
std::vector<std::string> getRequiredTags( const Context & context, const std::string& pattern="" );
    
//! Parses properties to build static component information.
//! Interfaces which are not in the config file will not appear here, e.g. Home, Tracer, Status.
orca::ComponentData getComponentData( const Context & context );

//@}
} // namespace

#endif
