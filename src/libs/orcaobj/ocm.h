/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2009 Alex Brooks, Alexei Makarenko, Tobias Kaupp, Ben Upcroft
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAOBJ_OCM_H
#define ORCAOBJ_OCM_H

#include <orca/ocm.h>

namespace orcaobj
{

/*!
 *  @name Stringify OCM Structures
 */
//@{

//! Converts fully qualified component name (aka Adapter ID) to a string.
//! e.i. platform/component
//! @see toComponentName
std::string toString( const orca::FQComponentName& );

//! Converts fully qualified interface name to a string.
//! e.i. interface\@platform/component
//! @see toInterfaceName
std::string toString( const orca::FQInterfaceName& );

//! Converts fully qualified topic name to a string.
//! i.e. interface/topic\@platform/component
std::string toString( const orca::FQTopicName& );

//! Converts fully executable name to a string.
//! i.e. executable@host
std::string toString( const orca::FQExecutableName & name );

//! Parses a string to extract a fully qualified component name (aka Adapter ID)
//! Expects an input string in the form of @c platform/component
//! A null structure is returned if no delimeter is present.
//! Does not handle the case of multiple delimeters.
orca::FQComponentName toComponentName( const std::string& s );

//! Parses a string to extract a fully qualified interface name
//! Expects an input string in the form of @c interface@platform/component
//! A null structure is returned if either one of the delimeters is
//! absent.
orca::FQInterfaceName toInterfaceName( const std::string& s );

//! Based on the component name, returns the default identity of Home interface.
std::string toHomeIdentity( const orca::FQComponentName & fqCName );

//! Based on the component name, returns the default topic name for Status interface.
orca::FQTopicName toStatusTopic( const orca::FQComponentName & fqCName );

//! Based on the component name, returns the default topic name for Tracer interface.
orca::FQTopicName toTracerTopic( const orca::FQComponentName & fqCName );


} // namespace

#endif
