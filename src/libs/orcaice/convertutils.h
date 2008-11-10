/*
 * Orca-Robotics Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2004-2008 Alex Brooks, Alexei Makarenko, Tobias Kaupp
 *
 * This copy of Orca is licensed to you under the terms described in
 * the LICENSE file included in this distribution.
 *
 */

#ifndef ORCAICE_CONVERT_UTILITIES_H
#define ORCAICE_CONVERT_UTILITIES_H


#include <orca/ocm.h>
#include <orcaice/context.h>

namespace orcaice
{
/*!
 *  @name Conversion Functions
 */
//@{

//! Extracts the fully-qualified component name.
//! No checks are performed.
orca::FQComponentName toComponent( const orca::FQInterfaceName& fqIName );

//! Returns the fully-qualified interface name.
//! No checks are performed.
orca::FQInterfaceName toInterface( const orca::FQComponentName& fqIName, const std::string &iface );

//! Returns the fully-qualified topic name.
//! No checks are performed.
orca::FQTopicName toTopic( const orca::FQInterfaceName& fqIName, const std::string& subtopic="*" );

//! Returns the fully-qualified topic name.
//! No checks are performed.
orca::FQTopicName toTopic( const orca::FQComponentName& fqCName, const std::string &iface, const std::string& subtopic="*" );

/*! 
Same as above but returns a string. Equivalent to calling:
@verbatim
orcaice::toString( orcaice::toTopic( component, ifaceName, subtopic ) );
@endverbatim
*/
std::string toTopicAsString( const orca::FQComponentName& component, const std::string &iface, const std::string& subtopic="*" );

//! Based on the component name, returns the default topic name for Status interface.
orca::FQTopicName toStatusTopic( const orca::FQComponentName& fqCName );

//! Based on the component name, returns the default topic name for Tracer interface.
orca::FQTopicName toTracerTopic( const orca::FQComponentName& fqCName );

//@}
} // namespace

#endif
