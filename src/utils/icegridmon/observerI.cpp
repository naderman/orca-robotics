/*
 * Orca Project: Components for robotics 
 *               http://orca-robotics.sf.net/
 * Copyright (c) 2006-2007 Alexei Makarenko
 *
 * This copy of Orca is licensed to you under the terms described in the
 * ORCA_LICENSE file included in this distribution.
 *
 */

#include "observerI.h"

#include <iostream>

using namespace std;

namespace icegridmon {

// std::string 
// icegridmon::toString( const IceGrid::AdapterInfo & obj )
// {
//     std::string s = "id=" + obj.id;
//     return s;
// }

NodeObserverI::NodeObserverI( const orcaice::Context & context ) :
    context_(context)
{
}

void
NodeObserverI::init(const ::IceGrid::NodeDynamicInfoSeq& nodes,
			     const Ice::Current& current)
{
    cout<<"NodeObserverI::init()"<<endl;
}

void
NodeObserverI::nodeUp(const ::IceGrid::NodeDynamicInfo& node,
			       const Ice::Current& current)
{
}

void
NodeObserverI::nodeDown(const ::std::string& name,
				 const Ice::Current& current)
{
}

void
NodeObserverI::updateServer(const ::std::string& node,
				     const ::IceGrid::ServerDynamicInfo& updatedInfo,
				     const Ice::Current& current)
{
}

void
NodeObserverI::updateAdapter(const ::std::string& node,
				      const ::IceGrid::AdapterDynamicInfo& updatedInfo,
				      const Ice::Current& current)
{
}

RegistryObserverI::RegistryObserverI( const orcaice::Context & context ) :
    context_(context)
{
}

void
RegistryObserverI::init(::Ice::Int serial,
				 const ::IceGrid::ApplicationDescriptorSeq& applications,
				 const ::IceGrid::AdapterInfoSeq& adpts,
				 const ::IceGrid::ObjectInfoSeq& objects,
				 const Ice::Current& current)
{
    cout<<"RegistryObserverI::init() serial="<<serial<<endl;
}

void
RegistryObserverI::applicationAdded(::Ice::Int serial,
					     const ::IceGrid::ApplicationDescriptor& desc,
					     const Ice::Current& current)
{
    context_.tracer()->info( "Added application" );
}

void
RegistryObserverI::applicationRemoved(::Ice::Int serial,
					       const ::std::string& name,
					       const Ice::Current& current)
{
}

void
RegistryObserverI::applicationUpdated(::Ice::Int serial,
					       const ::IceGrid::ApplicationUpdateDescriptor& desc,
					       const Ice::Current& current)
{
}

void
RegistryObserverI::adapterAdded(::Ice::Int serial,
					 const ::IceGrid::AdapterInfo& info,
					 const Ice::Current& current)
{
    context_.tracer()->info( "Added adapter id="+info.id );
}

void
RegistryObserverI::adapterUpdated(::Ice::Int serial,
					   const ::IceGrid::AdapterInfo& info,
					   const Ice::Current& current)
{
}

void
RegistryObserverI::adapterRemoved(::Ice::Int serial,
					   const ::std::string& id,
					   const Ice::Current& current)
{
}

void
RegistryObserverI::objectAdded(::Ice::Int serial,
					const ::IceGrid::ObjectInfo& info,
					const Ice::Current& current)
{
}

void
RegistryObserverI::objectUpdated(::Ice::Int serial,
					  const ::IceGrid::ObjectInfo& info,
					  const Ice::Current& current)
{
}

void
RegistryObserverI::objectRemoved(::Ice::Int serial,
					  const ::Ice::Identity& id,
					  const Ice::Current& current)
{
}

} // namespace
