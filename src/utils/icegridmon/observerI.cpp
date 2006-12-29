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

NodeObserverI::NodeObserverI( const orcaice::Context & context ) :
    context_(context)
{
}

void
NodeObserverI::init(const ::IceGrid::NodeDynamicInfoSeq& nodes,
			     const Ice::Current& current)
{
    cout<<"Node observer initialized"<<endl;
}

void
NodeObserverI::nodeUp(const ::IceGrid::NodeDynamicInfo& node,
			       const Ice::Current& current)
{
    context_.tracer()->info( "Added application" );
}

void
NodeObserverI::nodeDown(const ::std::string& name,
				 const Ice::Current& current)
{
    context_.tracer()->warning( "Node down "+name );
}

void
NodeObserverI::updateServer(const ::std::string& node,
				     const ::IceGrid::ServerDynamicInfo& updatedInfo,
				     const Ice::Current& current)
{
    string msg = "Updated server state=";
    stringstream ss;
    ss << "id="<<updatedInfo.id << " pid="<<updatedInfo.pid << " enabled="<<(int)updatedInfo.enabled;
    switch ( updatedInfo.state ) {
    case IceGrid::Inactive :
        context_.tracer()->info( msg+"Inactive "+ss.str() );
        break;
    case IceGrid::Activating :
        context_.tracer()->info( msg+"Activating "+ss.str() );
        break;
    case IceGrid::Active :
        context_.tracer()->info( msg+"Active "+ss.str() );
        break;
    case IceGrid::Deactivating :
        context_.tracer()->warning( msg+"Deactivating "+ss.str() );
        break;
    case IceGrid::Destroying :
        context_.tracer()->warning( msg+"Destroying "+ss.str() );
        break;
    case IceGrid::Destroyed :
        context_.tracer()->warning( msg+"Destroyed "+ss.str() );
        break;
    }
    
}

void
NodeObserverI::updateAdapter(const ::std::string& node,
				      const ::IceGrid::AdapterDynamicInfo& updatedInfo,
				      const Ice::Current& current)
{
    stringstream ss;
    ss << "Updated adapter id="<<updatedInfo.id<<" endpnts#="<<updatedInfo.proxy->ice_getEndpoints().size();
    context_.tracer()->info( ss.str() );
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
    cout<<"Registry observer initialized with serial="<<serial<<endl;
}

void
RegistryObserverI::applicationAdded(::Ice::Int serial,
					     const ::IceGrid::ApplicationDescriptor& desc,
					     const Ice::Current& current)
{
    context_.tracer()->info( "Added application " );
}

void
RegistryObserverI::applicationRemoved(::Ice::Int serial,
					       const ::std::string& name,
					       const Ice::Current& current)
{
    context_.tracer()->warning( "Removed application "+name );
}

void
RegistryObserverI::applicationUpdated(::Ice::Int serial,
					       const ::IceGrid::ApplicationUpdateDescriptor& desc,
					       const Ice::Current& current)
{
    context_.tracer()->info( "Updated application " );
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
    context_.tracer()->info( "Updated adapter id="+info.id );
}

void
RegistryObserverI::adapterRemoved(::Ice::Int serial,
					   const ::std::string& id,
					   const Ice::Current& current)
{
    context_.tracer()->warning( "Removed adapter id="+id );
}

void
RegistryObserverI::objectAdded(::Ice::Int serial,
					const ::IceGrid::ObjectInfo& info,
					const Ice::Current& current)
{
    context_.tracer()->info( "Added object type="+info.type );
}

void
RegistryObserverI::objectUpdated(::Ice::Int serial,
					  const ::IceGrid::ObjectInfo& info,
					  const Ice::Current& current)
{
    context_.tracer()->info( "Updated object type="+info.type );
}

void
RegistryObserverI::objectRemoved(::Ice::Int serial,
					  const ::Ice::Identity& id,
					  const Ice::Current& current)
{
    context_.tracer()->warning( "Removed object name="+id.name+ " cat="+id.category );
}

} // namespace
