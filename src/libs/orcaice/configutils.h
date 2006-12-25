/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2006 Alex Brooks, Alexei Makarenko, Tobias Kaupp
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

//! Reads properties contained in the context and returns the fully-qualified topic name
//! correpsonding to the interface tag.
//!
//! Throws ConfigFileException if the interface name cannot be read for some reason.
orca::FQTopicName getProvidedTopic( const Context &, const std::string &interfaceTag, const std::string & subtopic="*" );

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
std::vector<std::string> getProvidedTags( const Context & context );

//! For components with variable number of interfaces it may be useful to get a listing
//! of all required interfaces described in the configuration file.
std::vector<std::string> getRequiredTags( const Context & context );
    
//! Parses properties to build static component information.
//! Interfaces which are not in the config file will not appear here, e.g. Home, Tracer, Status.
orca::ComponentData getComponentData( const Context & context );

//@}
} // namespace

#endif
